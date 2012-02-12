#include <string.h>

int lesser(int i1, int i2){
  if(i1 > i2){
    return i2;
  } else {
    return i1;
  }
}

int rb_strcmp(char *str1, char *str2){
  long len;
  int retval;
  len = lesser(strlen(str1), strlen(str2));
  retval = memcmp(str1, str2, len);
  if (retval == 0){
    if (strlen(str1) == strlen(str2)) {      
      return 0;
    }
    if (strlen(str1) > strlen(str2)) return 1;
    return -1;
  }
  if (retval > 0) return 1;
  return -1;
}
