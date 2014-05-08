/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _RECOMMENDIFY_RANKEDITEMLIST_H
#define _RECOMMENDIFY_RANKEDITEMLIST_H

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unordered_map>

namespace recommendify {

/**
 * A ranked list of items
 */
class RankedItemList {
public:

  /**
   * Increment the score of an item in the ranked list. Will insert the item
   * into the list if it doesn't exist yet
   */
  void incrementItemScore(uint64_t item_key, double value);

protected:

  std::unordered_map<uint64_t, double> data_;

};

}
#endif
