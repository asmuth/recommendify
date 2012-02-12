#include <stdio.h>

#include "cc_item.h"
#include "version.h"

/**
 * \brief Print version string
 *
 * Print version string to standard output.
 */
void print_version(){
  printf(
    VERSION_STRING, 
    VERSION_MAJOR, 
    VERSION_MINOR, 
    VERSION_MICRO
  );
}

/**
 * \brief Print binary usage
 *
 * \param stream Output stream
 * \param bin    Binary name
 */
void print_usage(FILE *restrict stream, char const *restrict bin){
  fprintf(stream, USAGE_STRING, bin);
}

/**
 * \brief Print item structure
 *
 * \param item Item to print
 */
void print_item(struct cc_item const *restrict item){
  printf(
    "OUT: (%s) (%.4f)\n", 
    item->item_id, 
    item->similarity
  );
}
