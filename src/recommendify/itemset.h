/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2014 Paul Asmuth <paul@paulasmuth.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
   * Create a new item set. Takes ownership of the data vector
   *
   * @param data The items as a list of uint64_ts
   */
  ItemSet(std::vector<uint64_t>&& data) :
      items_(data) {}

  /**
   * Move an item set.
   */
  ItemSet(ItemSet&& other) :
      items_(std::move(other.items_)) {}

  /**
   * Cpoy an item set.
   */
  ItemSet(const ItemSet& other) :
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
