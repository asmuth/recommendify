/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include <random>
#include "minhash.h"

namespace recommendify {

MinHash::MinHash(
    uint64_t p,
    uint64_t q,
    const std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>& params) :
    p_(p),
    q_(q),
    params_(params) {
  assert(params_.size() == p * q);
};


std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> MinHash::generateParameters(
    uint64_t n,
    uint64_t m /* = 0x7fffffff */) {
  uint64_t i;
  std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> params;
  std::mt19937 prng((std::random_device())());
  std::uniform_int_distribution<> dist(1, m - 1);

  for (i = 0; i < n; ++i) {
    uint64_t a, b;

    a = dist(prng);
    b = dist(prng);

    params.push_back(std::tuple<uint64_t, uint64_t, uint64_t>(
        a, b, m));
  }

  return params;
}

std::vector<Fingerprint> MinHash::computeFingerprints(
    const ItemSet& input_set) const {
  std::vector<uint64_t> sig(p_, 0);
  std::vector<Fingerprint> fingerprints;

  for (int q = 0; q < q_; ++q) {
    for (int p = 0; p < p_; ++p) {
      sig[p] = computeMinHash(input_set, q * p_ + p);
    }

    fingerprints.push_back(Fingerprint(sig));
  }

  return fingerprints;
}

uint64_t MinHash::computeMinHash(
    const ItemSet& input_set,
    size_t index) const {
  uint64_t min = 0xffffffffffffffff;

  for (const uint64_t item : input_set.getItems()) {
    /* h(x) = (ax + b) % m */
    uint64_t h = (
        std::get<0>(params_[index]) *
        item +
        std::get<1>(params_[index])) % 
        std::get<2>(params_[index]);

    if (h < min) {
      min = h;
    }
  }

  return min;
}

const std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>&
    MinHash::getParameters() const {
  return params_;
}

uint64_t MinHash::getP() const {
  return p_;
}

uint64_t MinHash::getQ() const {
  return q_;
}

}
