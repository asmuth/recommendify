#include <stdio.h>
#include <string.h>
#include <hiredis/hiredis.h>

#include "version.h" 

int main (int argc, char **argv){
  redisContext *c;
  int similarityFunc = 0;
  /* redisReply *reply; */

  if(argc < 2) {
    printf(USAGE_STRING, argv[0]);
    return 1;
  }

  if(!strcmp(argv[1], "--version")){
    printf(VERSION_STRING, VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
    return 0;
  }

  if(!strcmp(argv[1], "--jaccard")) similarityFunc = 1;
  if(!strcmp(argv[1], "--cosine")) similarityFunc = 2;

  if(!similarityFunc){
    printf("invalid option: %s\n", argv[1]);
    return 1;
  }

  struct timeval timeout = { 1, 500000 }; 
  c = redisConnectWithTimeout("127.0.0.2", 6379, timeout); 

  if(c->err){
    printf("connection to redis failed: %s\n", c->errstr);
    return 1;
  }

  return 0;
}
