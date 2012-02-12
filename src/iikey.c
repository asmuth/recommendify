#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"

static size_t concat(char *restrict key, char const *restrict item1, char const *restrict item2) {
	size_t len1 = strlen(item1);
	size_t len2 = strlen(item2);

	memcpy(key, item1, len1);
	key[len1] = ':';
	memcpy(key + len1 + 1, item2, len2);
	key[len1 + 1 + len2] = '\0';

	return len1 + len2 + 2;
}

size_t item_item_key(char *restrict key, char const *restrict item1, char const *restrict item2){
  // FIXPAUL: make shure this does exactly the same as ruby sort  
  if (rb_strcmp(item1, item2) <= 0)
    return concat(key, item1, item2);
  else
    return concat(key, item2, item1);
}
