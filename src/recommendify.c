#include <stdio.h>
#include <string.h>
#include <hiredis/hiredis.h>

#include "version.h" 
#include "jaccard.c" 
#include "cosine.c" 
#include "output.c" 


int main(int argc, char **argv){
  int similarityFunc = 0;    
  char *itemID;  
  char *redisPrefix;
  char redisCmd[1024];
  redisContext *c;
  redisReply *all_items;
  
  
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


  /* prevent buffer overflows */ 
  if(strlen(redisPrefix) > 100)
    return 1; 

  if(strlen(itemID) > 100)
    return 1; 

  
  /* connect to redis */
  struct timeval timeout = { 1, 500000 }; 
  c = redisConnectWithTimeout("127.0.0.2", 6379, timeout); 

  if(c->err){
    printf("connection to redis failed: %s\n", c->errstr);
    return 1;
  }
 

  /* get all items */  
  sprintf(redisCmd, "HKEYS %s:items", redisPrefix); 
  printf("redis->exec: %s\n", redisCmd);

  all_items = redisCommand(c,redisCmd);

  if (all_items->type != REDIS_REPLY_ARRAY)
    return 1;


  /* calculate similarities */
  if(similarityFunc == 1)
    calculate_jaccard(c, redisPrefix, itemID, all_items);

  if(similarityFunc == 2)
    calculate_jaccard(c, redisPrefix, itemID, all_items);

  freeReplyObject(all_items);


  /* sort items by similarity */


  /* print the top x items */


  return 0;
}

