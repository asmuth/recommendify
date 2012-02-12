#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"

static char *concat(char const *restrict item1, char const *restrict item2) {
	size_t len1 = strlen(item1);
	size_t len2 = strlen(item2);

	char *key = malloc(len1 + len2 + 2);
	if (!key) {
		perror("malloc");
		return (char *) 0;
	}

	memcpy(key, item1, len1);
	key[len1] = ':';
	memcpy(key + len1 + 1, item2, len2);
	key[len1 + 1 + len2] = '\0';

	return key;
}

char *item_item_key(char const *restrict item1, char const *restrict item2){
  // FIXPAUL: make shure this does exactly the same as ruby sort  
  if (rb_strcmp(item1, item2) <= 0)
    return concat(item1, item2);
  else
    return concat(item2, item1);
}
