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
#include <stdlib.h>
#include <assert.h>
#include <random>
#include "minhash.h"

using namespace recommendify;

void testGenerateParameters() {
  std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> params;

  MinHash::generateParameters(params, 8);
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
  std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> params;
  std::vector<ItemSet> psets;
  std::vector<std::vector<Fingerprint>> pset_fingerprints;
  std::mt19937 prng((std::random_device())());
  std::uniform_int_distribution<> id_dist(44444, 999999);
  std::uniform_int_distribution<> card_dist(4, 50);

  p = 4;
  q = 10;

  MinHash::generateParameters(params, p * q);
  MinHash minhash(p, q, params);

  for (int j = 0; j < 100; ++j) {
    std::vector<uint64_t> pset_data;
    std::vector<Fingerprint> fingerprints;

    for (int i = card_dist(prng); i > 0; --i) {
      pset_data.push_back(id_dist(prng));
    }

    ItemSet pset(std::move(pset_data));

    minhash.computeFingerprints(pset, fingerprints);

    for (const auto& fingerprint : fingerprints) {
      std::string fingerprint_str = fingerprint.humanReadable();
      assert(fingerprint_str.size() > 16);
    }

    pset_fingerprints.push_back(fingerprints);
    psets.push_back(pset);
  }

  params.clear();
  minhash.getParameters(params);
  assert(params.size() == p * q);

  MinHash minhash2(minhash.getP(), minhash.getQ(), params);

  for (int j = 0; j < psets.size(); ++j) {
    std::vector<Fingerprint> new_fingerprints;
    minhash2.computeFingerprints(psets[j], new_fingerprints);

    for (int i = 0; i < pset_fingerprints[j].size(); i++) {
      assert(new_fingerprints[i].compareTo(pset_fingerprints[j][i]));
    }
  }
}

int main() {
  testGenerateParameters();
  testMinHash();
}
