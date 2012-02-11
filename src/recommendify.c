#include <stdio.h>
#include <string.h>
#include <hiredis/hiredis.h>

#include "version.h" 

void print_version(){
  printf(
    VERSION_STRING, 
    VERSION_MAJOR, 
    VERSION_MINOR, 
    VERSION_MICRO
  );
}

void print_usage(char *bin){
  printf(USAGE_STRING, bin);
}

int main(int argc, char **argv){
  redisContext *c;
  int similarityFunc = 0;
  /* redisReply *reply; */

  if(argc < 2){
    print_usage(argv[0]);
    return 1;
  }

  if(!strcmp(argv[1], "--version")){
    print_version();
    return 0;
  }

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

  struct timeval timeout = { 1, 500000 }; 
  c = redisConnectWithTimeout("127.0.0.2", 6379, timeout); 

  if(c->err){
    printf("connection to redis failed: %s\n", c->errstr);
    return 1;
  }

  return 0;
}

