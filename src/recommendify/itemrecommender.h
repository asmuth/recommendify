/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _RECOMMENDIFY_ITEMRECOMMENDER_H
#define _RECOMMENDIFY_ITEMRECOMMENDER_H

#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include "itemset.h"

namespace recommendify {

/**
 * An item recommender generates item<->item recommendations from from binary
 * user->item ratings. (i.e. 'User that viewed/bought/watched this, also viewed
 * bought/watched that)
 *
 * The input data is added by calling addPreferenceSet() repeatedly and must be
 * in the form of 'preference sets', i.e. sets of items that have been viewed/
 * bought/clicked by the same user (another way to look at preference sets is
 * as sparse binary feature/rating vectors)
 *
 * An item recommender supports only one type of query getRecommendations()
 * which returns a ranked list of items similar to a specified item.
 *
 * This is an abstract base class. Currently implemented subclasses are:
 *
 *   - JaccardRecommender
 *
 */
class ItemRecommender {
public:

  explicit ItemRecommender() {}
  explicit ItemRecommender(ItemRecommender& copy) = delete;

  /**
   * Add a preference set to the recommender
   */
  virtual void addPreferenceSet(const ItemSet& preference_set) = 0;

  /**
   * Retrieve a list of up to max_items ranked items most similar to the query
   * item and return them into the provided ranked item list
   */
  virtual void getRecommendations(
    uint64_t item_id,
    size_t max_items,
    RankedItemList* result) const = 0;

};

}
#endif
