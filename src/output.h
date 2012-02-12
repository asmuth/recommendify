#ifndef OUTPUT_H
#define OUTPUT_H

#include "cc_item.h"

void print_version();
void print_usage(FILE *restrict, char const *restrict bin);
void print_item(struct cc_item const *restrict item);

#endif
