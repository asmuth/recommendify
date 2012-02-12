#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>

#include "output.h"

struct cc_item {         
  char  item_id[64]; /* FIXPAUL */
  int   coconcurrency_count;
  int   total_count;
  float similarity;  
};         


// FIXPAUL: I think this reeks...
char* item_item_key(char *item1, char *item2){
  //char *key = "fnord";
  //printf("foo: %s, %s\n", item1, item2);
  char *key = (char *)malloc(strlen(item1) + strlen(item2) + 1);

  if(!key)
    return "fnord";

  // FIXPAUL: make shure this does exactly the same as ruby sort  
  if(strcmp(item1, item2) < 0){
    sprintf(key, "%s:%s", item1, item2);
  } else {
    sprintf(key, "%s:%s", item2, item1);
  }

  return key;
}

int main(int argc, char **argv){
  int j, similarityFunc = 0;    
  char *itemID;  
  char *redisPrefix;
  char redisCmd[1024]; // FIXPAUL: use hiredis format strings
  redisContext *c;
  redisReply *all_items;
  redisReply *reply;
  
  
  /* option parsing */
  if(argc < 2) {
    print_usage(argv[0]);
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
    printf("invalid option: %s\n", argv[1]);
    return EXIT_FAILURE;
  } else if(argc != 4){
    printf("wrong number of arguments\n");
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  redisPrefix = argv[2];
  itemID = argv[3];


  /* FIXPAUL gprevent buffer overflows */ 
  if(strlen(redisPrefix) > 100)
    return EXIT_FAILURE;

  if(strlen(itemID) > 100)
    return EXIT_FAILURE;

  
  /* connect to redis */
  struct timeval timeout = { 1, 500000 }; 
  c = redisConnectWithTimeout("127.0.0.2", 6379, timeout); 

  if(c->err){
    printf("connection to redis failed: %s\n", c->errstr);
    return EXIT_FAILURE;
  }
 

  /* get all items OPTIMIZE: get total counts */  
  sprintf(redisCmd, "HKEYS %s:items", redisPrefix);  /* fixme: use snprintf */
  printf("redis->exec: %s\n", redisCmd); /* fixme use hiredis format string */

  all_items = redisCommand(c,redisCmd);

  if (all_items->type != REDIS_REPLY_ARRAY)
    return EXIT_FAILURE;


  /* populate the cc_items array */ 
  int cc_items_size = all_items->elements;  
  int cc_items_mem = cc_items_size * sizeof(struct cc_item);
  struct cc_item *cc_items = malloc(cc_items_mem);

  if(!cc_items){    
    printf("cannot allocate memory: %i", cc_items_mem);
    return EXIT_FAILURE;
  }
  
  for (j = 0; j < all_items->elements; j++){      
    /* FIXPAUL: buffer overflow: make char_id dynamic and find longest_id for malloc */
    strcpy(cc_items[j].item_id, all_items->element[j]->str);
  }

  freeReplyObject(all_items);



  /* get all item data from redis (OPTIMIZE with hmget) */
  for (j = 0; j < cc_items_size; j++){

    /* get total count (OPTIMIZE: get with the first hkeys/hgetall) */
    reply = redisCommand(c,"HGET %s:items %s", redisPrefix, cc_items[j].item_id);  
    cc_items[j].total_count = atoi(reply->str);    
    freeReplyObject(reply);

    char *iikey = item_item_key(itemID, cc_items[j].item_id);        
    reply = redisCommand(c,"HGET %s:ccmatrix %s", redisPrefix, iikey);  
    if(reply->str){
      printf("res: %s\n", reply->str);
      cc_items[j].coconcurrency_count = atoi(reply->str);
    } else {
      cc_items[j].coconcurrency_count = 0;
    }
    
    freeReplyObject(reply);
    free(iikey);

    /*printf(
      "item: %i -> %s (ccn: %i, ttl: %i) \n", j, 
      cc_items[j].item_id,
      cc_items[j].coconcurrency_count,
      cc_items[j].total_count
    );*/
  }  


  /* calculate similarities */
  //if(similarityFunc == 1)
  //  calculate_jaccard(c, redisPrefix, itemID, cc_items);

  //if(similarityFunc == 2)
  //  calculate_jaccard(c, redisPrefix, itemID, cc_items);

  
  /* sort items by similarity */


  /* print the top x items */


  free(cc_items);

  printf("bye\n");

  return EXIT_SUCCESS;
}


