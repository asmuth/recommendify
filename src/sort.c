#include <stddef.h>

/**
 * \brief Compare two strings in rubyesque manner
 *
 * \param str1 First string
 * \param str2 Second string
 *
 * \return Something rubyesque
 */
int rb_strcmp(char const *restrict str1, char const *restrict str2) {
  for (size_t i = 0;; ++i) {
    if (!str1[i]) {
      if (!str2[i])
        /* Strings of equal length */
        return 0;
      else
        /* str1 shorter */
        return -1;
    }
    else if (!str2[i]) {
      /* str1 longer */
      return 1;
    }
    else if (str1[i] != str2[i]) {
      /* First unequal pair */
      return str1[i] > str2[i] ? 1 : -1;
    }
  }
}
