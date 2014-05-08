/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include <random>
#include "rankeditemlist.h"

namespace recommendify {

void RankedItemList::incrementItemScore(uint64_t item_key, double value) {
  data_[item_key] += value;
}

}
