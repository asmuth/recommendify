#include <hiredis/hiredis.h>

#include "cc_item.h"

void calculate_jaccard(char const *restrict item_id, size_t itemCount, struct cc_item *restrict cc_items, size_t cc_items_size) {
  for (size_t j = 0; j < cc_items_size; j++) {
    size_t n = cc_items[j].coconcurrency_count;
    if (n > 0)
      cc_items[j].similarity =
        (float) n / (
          (float) itemCount +
          (float) cc_items[j].total_count -
          (float) n
        );
    else
      cc_items[j].similarity = 0.0;
  }
}
