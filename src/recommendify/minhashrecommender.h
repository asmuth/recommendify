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
#ifndef _RECOMMENDIFY_MINHASHRECOMMENDER_H
#define _RECOMMENDIFY_MINHASHRECOMMENDER_H

#include <stdlib.h>
#include <stdint.h>
#include "itemset.h"
#include "minhash.h"
#include "userrecommender.h"

namespace recommendify {

/**
 * The minhash recommender generates personalized recommendations by assigning
 * each preference set a number of fingerprints using the MinHash similarity
 * hashing scheme (see minhash.h).
 *
 */
class MinHashRecommender : public UserRecommender {
public:

  /**
   * Construct a new MinHashRecommender. You must provide a MinHash instance
   * (see minhash.h).
   */
  MinHashRecommender(
      const MinHash* minhash);

  MinHashRecommender(MinHashRecommender& copy) = delete;

  /**
   * Add a preference set to the recommender
   */
  virtual void addPreferenceSet(const ItemSet& preference_set) override;

  /**
   * Retrieve a list of up to max_items ranked items most similar to the query
   * point and return them into the provided ranked item list
   */
  virtual void getRecommendations(
      const ItemSet& query_point,
      size_t max_items,
      RankedItemList& result) const override;

protected:

  MinHash minhash_;

};

}
#endif
