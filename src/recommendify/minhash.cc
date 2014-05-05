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

void MinHash::generateParameters(
    std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>& destination,
    uint64_t n,
    uint64_t m /* = 0x7fffffff */) {
  uint64_t i;

  std::mt19937 prng((std::random_device())());
  std::uniform_int_distribution<> dist(1, m - 1);

  for (i = 0; i < n; ++i) {
    uint64_t a, b;

    a = dist(prng);
    b = dist(prng);

    destination.push_back(std::tuple<uint64_t, uint64_t, uint64_t>(
        a, b, m));
  }
}

void MinHash::computeFingerprints(
    const std::vector<uint64_t>& input_set,
    std::vector<Fingerprint>& dest) const {

  for (int q = 0; q < q_; ++q) {
    std::vector<uint64_t> sig(p_, 0);

    for (int p = 0; p < p_; ++p) {
      sig[p] = computeMinHash(input_set, q * p_ + p);
    }

    dest.push_back(Fingerprint(std::move(sig)));
  }
}

uint64_t MinHash::computeMinHash(
    const std::vector<uint64_t>& input_set,
    size_t index) const {
  uint64_t min = 0xffffffffffffffff;

  for (const uint64_t item : input_set) {
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

void MinHash::getParameters(
    std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>& destination) const {

  for (const auto& param : params_) {
    destination.push_back(param);
  }
}

uint64_t MinHash::getP() const {
  return p_;
}

uint64_t MinHash::getQ() const {
  return q_;
}

}
