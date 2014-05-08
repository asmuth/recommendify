/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include <random>
#include "minhashrecommender.h"

namespace recommendify {

void MinHashRecommender::addPreferenceSet(const ItemSet& preference_set) {
  std::vector<Fingerprint> fingerprints;

  fingerprints = minhash_.computeFingerprints(preference_set);

  for (const uint64_t item : preference_set.getItems()) {
    for (const auto& fingerprint : fingerprints) {
      // FIXPAUL humanReadable is slow slow slow
      backend_.incrementBy(fingerprint.humanReadable(), item, 1.0f);
    }
  }
}

void MinHashRecommender::getRecommendations(const ItemSet& query_point,
    size_t max_items, RankedItemList* result) const {
  // printf("get recos %i\n", query_point.getItems().size());
}

}
