/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _RECOMMENDIFY_USERRECOMMENDER_H
#define _RECOMMENDIFY_USERRECOMMENDER_H

#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include "itemset.h"
#include "rankeditemlist.h"

namespace recommendify {

/**
 * A user recommender generates personalized recommendations from binary
 * user->item ratings
 *
 * The input data is added by calling addPreferenceSet() repeatedly and must be
 * in the form of 'preference sets', i.e. sets of items that have been viewed/
 * bought/clicked by the same user (another way to look at preference sets is
 * as sparse binary feature/rating vectors)
 *
 * A user recommender supports only one type of query getRecommendations() which
 * returns a ranked list of items similar to the query point. The query point is
 * also also a preference set (the set of items which the user for which the 
 * recommendation is to be made viewed/bought/clicked).
 *
 * This is an abstract base class. Currently implemented subclasses are:
 *
 *   - MinHashRecommender
 *
 */
class UserRecommender {
public:

  UserRecommender() {}
  UserRecommender(UserRecommender& copy) = delete;

  /**
   * Add a preference set to the recommender
   */
  virtual void addPreferenceSet(const ItemSet& preference_set) = 0;

  /**
   * Retrieve a list of up to max_items ranked items most similar to the query
   * point and return them into the provided ranked item list
   */
  virtual void getRecommendations(
      const ItemSet& query_point,
      size_t max_items,
      RankedItemList* result) const = 0;

};

}
#endif
