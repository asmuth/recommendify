/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _RECOMMENDIFY_MEMORYBACKEND_H
#define _RECOMMENDIFY_MEMORYBACKEND_H

#include <stdlib.h>
#include <stdint.h>
#include <unordered_map>
#include "itemset.h"
#include "rankeditemlist.h"

namespace recommendify {

/**
 * A storage backend that keeps all data in memory
 */
class MemoryBackend {
public:

  /**
   * Create a new MemoryBackend
   */
  explicit MemoryBackend() {}

  /**
   * Move a memory backend
   */
  explicit MemoryBackend(MemoryBackend&& other) :
      data_(std::move(other.data_)) {}

  MemoryBackend(MemoryBackend& copy) = delete;

  /**
   * Increment the score of an element in a ranked list
   */
  void incrementBy(
      const std::string& list_key,
      uint64_t item_key,
      double value);

protected:

  std::unordered_map<std::string, RankedItemList> data_;

};

}
#endif
