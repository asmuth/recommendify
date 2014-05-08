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

using namespace recommendify;

void testGenerateParameters() {
  auto params = MinHash::generateParameters(8);
  assert(params.size() == 8);

  for (auto& param : params) {
    assert(std::get<0>(param) >  0);
    assert(std::get<0>(param) <  0x7fffffff);
    assert(std::get<1>(param) >  0);
    assert(std::get<1>(param) <  0x7fffffff);
    assert(std::get<2>(param) == 0x7fffffff);
  }
}

void testMinHash() {
  uint64_t p, q;
  std::vector<ItemSet> psets;
  std::vector<std::vector<Fingerprint>> pset_fingerprints;
  std::mt19937 prng((std::random_device())());
  std::uniform_int_distribution<> id_dist(44444, 999999);
  std::uniform_int_distribution<> card_dist(4, 50);

  p = 4;
  q = 10;

  MinHash minhash(p, q, MinHash::generateParameters(p * q));
  assert(minhash.getParameters().size() == p * q);

  for (int j = 0; j < 100; ++j) {
    std::vector<uint64_t> pset_data;

    for (int i = card_dist(prng); i > 0; --i) {
      pset_data.push_back(id_dist(prng));
    }

    ItemSet pset(std::move(pset_data));
    auto fingerprints = minhash.computeFingerprints(pset);

    for (const auto& fingerprint : fingerprints) {
      auto fingerprint_str = fingerprint.humanReadable();
      assert(fingerprint_str.size() > 16);
    }

    pset_fingerprints.push_back(fingerprints);
    psets.push_back(pset);
  }

  MinHash minhash2(
      minhash.getP(),
      minhash.getQ(),
      minhash.getParameters());

  for (int j = 0; j < psets.size(); ++j) {
    auto new_fingerprints = minhash2.computeFingerprints(psets[j]);

    for (int i = 0; i < pset_fingerprints[j].size(); i++) {
      assert(new_fingerprints[i].compareTo(pset_fingerprints[j][i]));
    }
  }
}

int main() {
  testGenerateParameters();
  testMinHash();
}
