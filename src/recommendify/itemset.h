/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _RECOMMENDIFY_ITEMSET_H
#define _RECOMMENDIFY_ITEMSET_H

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <vector>

namespace recommendify {

/**
 * An item set / preference set. Usually contains a list of item ids that have
 * been viewed/bought/clicked/etc together (i.e. by the same user).
 */
class ItemSet {
public:

  /**
   * Create a new item set and copy the data
   *
   * @param data The items as a list of uint64_ts
   */
  explicit ItemSet(const std::vector<uint64_t>& data) :
      items_(data) {}

  /**
   * Create a new item set and move the data
   *
   * @param data The items as a list of uint64_ts
   */
  explicit ItemSet(std::vector<uint64_t>&& data) :
      items_(std::move(data)) {}

  /**
   * Move an item set.
   */
  explicit ItemSet(ItemSet&& other) :
      items_(std::move(other.items_)) {}

  /**
   * Copy an item set.
   */
  explicit ItemSet(const ItemSet& other) :
    items_(other.items_) {}

  /**
   * Retrieve a reference to the items vector
   */
  const std::vector<uint64_t>& getItems() const {
    return items_;
  }

protected:

  std::vector<uint64_t> items_;

};

}
#endif
