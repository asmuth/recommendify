#include <stdio.h>

#include "version.h"

void print_version(){
  printf(
    VERSION_STRING, 
    VERSION_MAJOR, 
    VERSION_MINOR, 
    VERSION_MICRO
  );
}

void print_usage(char const *restrict bin){
  printf(USAGE_STRING, bin);
}

void print_item(struct cc_item item){
  printf(
    "OUT: (%s) (%.4f)\n", 
    item.item_id, 
    item.similarity
  );
}
