#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>

#include "version.h" 
#include "cc_item.h" 
#include "jaccard.c" 
#include "cosine.c" 
#include "output.c" 
#include "sort.c" 
#include "iikey.c" 

/* 
  FIXPAUL: MAX_ID_LENGTH = 64
*/

int main(int argc, char **argv){
  int i, j, similarityFunc = 0;    
  int itemCount = 0;
  char *itemID;  
  char *redisPrefix;
  redisContext *c;
  redisReply *all_items;
  redisReply *reply;
  
  
  /* option parsing */
  if(argc < 2)
    return print_usage(argv[0]);

  if(!strcmp(argv[1], "--version"))
    return print_version();

  if(!strcmp(argv[1], "--jaccard")) 
    similarityFunc = 1;

  if(!strcmp(argv[1], "--cosine"))  
    similarityFunc = 2;

  if(!similarityFunc){
    printf("invalid option: %s\n", argv[1]);
    return 1;
  } else if(argc != 4){
    printf("wrong number of arguments\n");
    print_usage(argv[0]);
    return 1;
  }

  redisPrefix = argv[2];
  itemID = argv[3];


  /* connect to redis */
  struct timeval timeout = { 1, 500000 }; 
  c = redisConnectWithTimeout("127.0.0.2", 6379, timeout); 

  if(c->err){
    printf("connection to redis failed: %s\n", c->errstr);
    return 1;
  }
 

  /* get all items OPTIMIZE: get total counts */  
  all_items = redisCommand(c,"HKEYS %s:items", redisPrefix);

  if(all_items->type != REDIS_REPLY_ARRAY)
    return 1;


  /* get item count (OPTIMIZE: get with the first hkeys/hgetall) */
  reply = redisCommand(c,"HGET %s:items %s", redisPrefix, itemID);    
  itemCount = atoi(reply->str);    
  freeReplyObject(reply);

  if(itemCount == 0){
    printf("item count is zero\n");
    return 0;
  }
  

  /* populate the cc_items array */ 
  int cc_items_size = all_items->elements;
  int cc_items_mem = cc_items_size * sizeof(struct cc_item);
  struct cc_item *cc_items = malloc(cc_items_mem);

  cc_items_size--;

  if(!cc_items){    
    printf("cannot allocate memory: %i", cc_items_mem);
    return 1;
  }
  
  i = 0;
  for (j = 0; j < all_items->elements; j++){          
    if(strcmp(itemID, all_items->element[j]->str) != 0){
      /* FIXPAUL: make char_id dynamic and find longest_id for malloc */
      strncpy(cc_items[i].item_id, all_items->element[j]->str, 64);  
      i++;
    }    
  }

  freeReplyObject(all_items);

  int hits = 0;

  /* get all item data from redis: OPTIMIZE: get in batches with hmget */
  for (j = 0; j < cc_items_size; j++){

    /* get total count (OPTIMIZE: get with the first hkeys/hgetall) */
    reply = redisCommand(c,"HGET %s:items %s", redisPrefix, cc_items[j].item_id);      
    cc_items[j].total_count = atoi(reply->str);    
    freeReplyObject(reply);
    
    /* OPTIMIZE: get in batches with hmget */ 
    char *iikey = item_item_key(itemID, cc_items[j].item_id);
    //printf("fnord: %s vs %s -> %s\n", itemID, cc_items[j].item_id, iikey);
    reply = redisCommand(c,"HGET %s:ccmatrix %s", redisPrefix, iikey);  
    if(reply->str){
      printf("res: %s\n", reply->str);
      hits += 1;
      cc_items[j].coconcurrency_count = atoi(reply->str);
    } else {
      cc_items[j].coconcurrency_count = 0;
    }
    
    i++;

    freeReplyObject(reply);

    if(iikey)
      free(iikey);

    /*printf(
      "item: %i -> %s (ccn: %i, ttl: %i) \n", j, 
      cc_items[j].item_id,
      cc_items[j].coconcurrency_count,
      cc_items[j].total_count
    );*/
  }  

  printf("item count: %i\n", itemCount);
  printf("hits: %i\n", hits);

  /* calculate similarities */
  if(similarityFunc == 1)
    calculate_jaccard(itemID, itemCount, cc_items, cc_items_size);

  //if(similarityFunc == 2)
  //  calculate_jaccard(c, redisPrefix, itemID, cc_items);

  
  /* sort items by similarity */


  /* print the top x items */


  free(cc_items); 

  printf("bye\n");

  return 0;
}


