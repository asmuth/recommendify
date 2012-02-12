#include <hiredis/hiredis.h>

void calculate_jaccard(redisContext *restrict c, char const *restrict redisPrefix, char const *restrict item_id, redisReply *restrict all_items){
  int j;
  printf("calculate_jaccard\n");
  for (j = 0; j < all_items->elements; j++){
    printf("%u) %s\n", j, all_items->element[j]->str);
  }
}
