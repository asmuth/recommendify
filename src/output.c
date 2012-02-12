#include <stdio.h>

#include "cc_item.h"
#include "version.h"

void print_version(){
  printf(
    VERSION_STRING, 
    VERSION_MAJOR, 
    VERSION_MINOR, 
    VERSION_MICRO
  );
}

void print_usage(FILE *restrict stream, char const *restrict bin){
  fprintf(stream, USAGE_STRING, bin);
}

void print_item(struct cc_item const *restrict item){
  printf(
    "OUT: (%s) (%.4f)\n", 
    item->item_id, 
    item->similarity
  );
}
