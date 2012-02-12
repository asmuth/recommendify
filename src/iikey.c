#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"

char* item_item_key(char *item1, char *item2){
  int keylen = strlen(item1) + strlen(item2) + 2;
  char *key = (char *)malloc(keylen * sizeof(char));

  if(!key){
    printf("cannot allocate\n");
    return 0;
  }  

  // FIXPAUL: make shure this does exactly the same as ruby sort  
  if(rb_strcmp(item1, item2) <= 0){
    snprintf(key, keylen, "%s:%s", item1, item2);
  } else {
    snprintf(key, keylen, "%s:%s", item2, item1);
  }

  return key;
}
