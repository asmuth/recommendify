#ifndef JACCARD_H
#define JACCARD_H

#include "cc_item.h"

void calculate_jaccard(char const *restrict item_id, int itemCount, struct cc_item *restrict cc_items, int cc_items_size);

#endif
