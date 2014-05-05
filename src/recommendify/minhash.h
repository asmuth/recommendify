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
#ifndef _RECOMMENDIFY_MINHASH_H
#define _RECOMMENDIFY_MINHASH_H

#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <tuple>
#include <string>
#include "lsh.h"
#include "fingerprint.h"

namespace recommendify {

/**
 * An implementation of the MinHash similiarity hashing scheme. This class
 * fingerprints preference sets of uint64 ids by assigning each input item set
 * Q different fingerprints. Each fingerprint is obtained by concatting the
 * minimum values of P min-wise independent permutations of the input set.
 *
 * The P*Q required permutations of the input set are constructed by hashing
 * the input set with P*Q different hash functions. This is done using
 * tabulation hashing and a list of P*Q different parameter tuples (a,b,m).
 *
 *    h(x) = (ax + b) % m
 *
 * All methods of this class are threadsafe.
 */
class MinHash : public LSH {
public:

  /**
   * Create a new MinHash processor.
   *
   * The two params p and q control how many minhashes will be calculated for
   * each fingerprint and how many fingerprints will be calculated per input set
   * respectively. The total number of computed minhashes will be p * q.
   *
   * This constructor requires a list of p * q parameter tuples (a,b,m) for the
   * tabulation hash functions.
   *
   * Good values to start experimenting are:
   *   p = {1..4}
   *   q = {1..100}
   *
   * @param p      The number of individual minhashes per fingerprint
   * @param q      The number of fingerprints to calculate per input set
   * @param params The list of parameters for the p*q tabulation hash functions
   */
  MinHash(
      uint64_t p,
      uint64_t q,
      const std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>& params);

  MinHash(MinHash& copy) = delete;

  /**
   * Generate a list of N parameter sets (a,b,m) for tabulation hashing and
   * store them in the destination vector.
   *
   * @param n the number of parameter tuples to generate
   * @param m a large prime number
   */
  static void generateParameters(
      std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>& destination,
      uint64_t n,
      uint64_t m = 0x7fffffff);

  /**
   * Compute a list of fingerprints for the input item set and store them in
   * the destination vector.
   *
   * This will return Q binary strings consisting of P concatenated minhash
   * values each into the destination vector
   */
  void computeFingerprints(
      const std::vector<uint64_t>& input_set,
      std::vector<Fingerprint>& dest) const;

  /**
   * Returns the tabulation hashing parameter sets ({a,b,m} tuples) into the
   * destination vector
   */
  void getParameters(
      std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>& destination) const;

  /**
   * returns P, the number of minhashes per fingerprint
   */
  uint64_t getP() const;

  /**
   * returns Q, the number of fingerprints per input set
   */
  uint64_t getQ() const;

protected:

  std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> params_;
  uint64_t p_;
  uint64_t q_;

  /**
   * Compute a single minhash for an input set using one of the permutations /
   * parameter sets
   *
   * @param input_set the input set to compute the minhash for
   * @param index     the index of the parameter tuple to be used
   */
  uint64_t computeMinHash(
      const std::vector<uint64_t>& input_set,
      size_t index) const;

};

}
#endif
