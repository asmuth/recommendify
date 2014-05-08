/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include <random>
#include "memorybackend.h"

namespace recommendify {

void MemoryBackend::incrementBy(const std::string& list_key, uint64_t item_key,
    double value) {
  data_[list_key].incrementItemScore(item_key, value);
}

}
