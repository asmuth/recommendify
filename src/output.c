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

void print_usage(char *bin){
  printf(USAGE_STRING, bin);
}
