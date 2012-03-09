#ifndef JACCARD_H
#define JACCARD_H

#include "cc_item.h"

void calculate_jaccard(char const *restrict item_id, size_t itemCount, struct cc_item *restrict cc_items, size_t cc_items_size);

#endif
