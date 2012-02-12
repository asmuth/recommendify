#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <hiredis/hiredis.h>

#include "cc_item.h" 
#include "cosine.h"
#include "iikey.h"
#include "jaccard.h"
#include "output.h"

int main(int argc, char **argv){
  size_t i, j, n;
  int similarityFunc = 0;
  size_t itemCount = 0;
  char *itemID;  
  char *redisPrefix;
  redisContext *c;
  redisReply *all_items;
  redisReply *reply;
  size_t cur_batch_size;
  char* cur_batch;
  char *iikey;

  size_t batch_size = 200; /* FIXPAUL: make option */
  size_t maxItems = 50; /* FIXPAUL: make option */
  
  
  /* option parsing */
  if(argc < 2) {
    print_usage(stderr, argv[0]);
    return EXIT_FAILURE;
  }

  if(!strcmp(argv[1], "--version")) {
    print_version();
    return EXIT_SUCCESS;
  }

  else if(!strcmp(argv[1], "--jaccard")) 
    similarityFunc = 1;

  else if(!strcmp(argv[1], "--cosine"))  
    similarityFunc = 2;

  if(!similarityFunc){
    fprintf(stderr, "Invalid option: %s\n", argv[1]);
    return EXIT_FAILURE;
  } else if(argc != 4){
    fputs("Wrong number of arguments\n", stderr);
    print_usage(stderr, argv[0]);
    return EXIT_FAILURE;
  }

  redisPrefix = argv[2];
  itemID = argv[3];


  /* connect to redis */
  struct timeval timeout = { 1, 500000 }; 
  c = redisConnectWithTimeout("127.0.0.2", 6379, timeout); 

  if(c->err){
    fprintf(stderr, "Connection to redis failed: %s\n", c->errstr);
    return EXIT_FAILURE;
  }
 

  /* get item count */
  reply = redisCommand(c,"HGET %s:items %s", redisPrefix, itemID);    
  itemCount = atoi(reply->str);    
  freeReplyObject(reply);

  if(itemCount == 0){
    fputs("Item count is zero\n", stderr);
    return EXIT_SUCCESS;
  }
  

  /* get all items_ids and the total counts */ 
  all_items = redisCommand(c,"HGETALL %s:items", redisPrefix);

  if (all_items->type != REDIS_REPLY_ARRAY)
    return EXIT_FAILURE;


  /* populate the cc_items array */ 
  size_t cc_items_size = all_items->elements / 2;  
  size_t cc_items_mem = cc_items_size * sizeof(struct cc_item);
  struct cc_item *cc_items = malloc(cc_items_mem);
  cc_items_size--;

  if(!cc_items){    
    fprintf(stderr, "Cannot allocate %lu bytes of memory: %s",
      cc_items_mem, strerror(errno));
    return EXIT_FAILURE;
  }
  
  i = 0;
  for (j = 0; j < all_items->elements/2; j++){                   
    if(strcmp(itemID, all_items->element[j*2]->str) != 0){      
      strncpy(cc_items[i].item_id, all_items->element[j*2]->str, ITEM_ID_SIZE);
      cc_items[i].total_count = atoi(all_items->element[j*2+1]->str);
      i++;
    }
  }

  freeReplyObject(all_items);


  // batched redis hmgets on the ccmatrix
  cur_batch = malloc(batch_size * (ITEM_ID_SIZE + 4) * 2);

  if(!cur_batch){    
    perror("Cannot allocate memory");
    return EXIT_FAILURE;
  }

  n = cc_items_size;
  while(n >= 0){
    cur_batch_size = ((n-1 < batch_size) ? n-1 : batch_size);

    for(i = 0; i < cur_batch_size; i++){
      iikey = item_item_key(itemID, cc_items[n-i].item_id);

      strcat(cur_batch, iikey);
      strcat(cur_batch, " ");

      if(iikey)
        free(iikey);
    }    

    redisAppendCommand(c, "HMGET %s:ccmatrix %s", redisPrefix, cur_batch);  
    redisGetReply(c, (void**)&reply);
      
    for(j = 0; j < reply->elements; j++){
      if(reply->element[j]->str){
        cc_items[n-j].coconcurrency_count = atoi(reply->element[j]->str);
      } else {
        cc_items[n-j].coconcurrency_count = 0;
      }   
    }

    freeReplyObject(reply);
    n -= batch_size;
  }

  free(cur_batch);



  /* calculate similarities */
  if(similarityFunc == 1)
    calculate_jaccard(itemID, itemCount, cc_items, cc_items_size);

  if(similarityFunc == 2)
    calculate_cosine(itemID, itemCount, cc_items, cc_items_size);

  
  /* find the top x items with simple bubble sort */
  for(i = 0; i < maxItems - 1; ++i){
    for (j = 0; j < cc_items_size - i - 1; ++j){
      if (cc_items[j].similarity > cc_items[j + 1].similarity){
        struct cc_item tmp = cc_items[j];
        cc_items[j] = cc_items[j + 1];
        cc_items[j + 1] = tmp;
      }
    }
  }


  /* print top k items */
  n = ((cc_items_size < maxItems) ? cc_items_size : maxItems);
  for(j = 0; j < n; j++){
    i = cc_items_size-j-1;
    if(cc_items[i].similarity > 0){
      print_item(cc_items + i);      
    }    
  }


  free(cc_items);
  return EXIT_SUCCESS;
}


