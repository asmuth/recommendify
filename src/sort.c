#include <string.h>

static size_t lesser(size_t i1, size_t i2){
  return i1 > i2 ? i2 : i1;
}

int rb_strcmp(char const *restrict str1, char const *restrict str2) {
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);

  size_t len = lesser(len1, len2);

  int ret = memcmp(str1, str2, len);
  if (ret == 0) {
    if (len1 == len2)
      return 0;
    else if (len1 > len2)
      return 1;
    else
      return -1;
  }
  else
    return ret > 0 ? 1 : -1;
}
