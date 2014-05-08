/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _RECOMMENDIFY_MINHASHRECOMMENDER_H
#define _RECOMMENDIFY_MINHASHRECOMMENDER_H

#include <stdlib.h>
#include <stdint.h>
#include "itemset.h"
#include "minhash.h"
#include "userrecommender.h"
#include "memorybackend.h"

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
   * (see minhash.h) and a storage backend. Takes ownership of the minhash and
   * the backend.
   */
  explicit MinHashRecommender(
      MinHash&& minhash,
      MemoryBackend&& backend) :
      minhash_(std::move(minhash)),
      backend_(std::move(backend)) {};

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
      RankedItemList* result) const override;

protected:

  const MinHash minhash_;
  MemoryBackend backend_;

};

}
#endif
