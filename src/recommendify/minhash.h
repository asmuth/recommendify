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

namespace recommendify {

/**
 * An implementation of the MinHash similiarity hashing scheme. This class
 * fingerprints preference sets of uint64 ids by assigning each preference set
 * fingerprint Q different hash signatures. Each hash signature is obtained by
 * concatting the minimum values of P min-wise independent permutations of the
 * input set.
 *
 * The P*Q required permutations of the input set are constructed by hashing
 * the input set with P*Q different hash functions. This is done using
 * tabulation hashing and a list of P*Q different parameter tuples (a,b,p).
 *
 *    h(x) = (ax + b) % p
 *
 */
class MinHash : public LSH {
public:

  /**
   * Create a new MinHash processor.
   *
   * The two params p and q control how many minhashes will be calculated for
   * each signature and how many signatures will be calculated per input set
   * respectively. The total number of computed hashes will be p * q.
   *
   * This constructor requires a list of p*q parameter tuples (a,b,p) for the
   * tabulation hash functions.
   *
   * Good values to start experimenting are:
   *   p = {1..4}
   *   q = {1..100}
   *
   * @param p      The number of individual minhashes per hash signature
   * @param q      The number of hash signatures to calculate per input set
   * @param params The list of parameters for the p*q tabulation hash functions
   */
  MinHash(
      uint64_t p,
      uint64_t q,
      const std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>& params);

  MinHash(MinHash& copy) = delete;

  /**
   * Compute a list of hash signatures for the input item set and store them in
   * the destination vector
   */
  void computeHashSignatures(
      const std::vector<uint64_t>& input_set,
      std::vector<std::string>& dest) const;

  /**
   * Returns the tabulation hashing parameter sets ({a,b,p} tuples) into the 
   * destination vector
   */
  void getCoefficients(
      std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>& destination) const;

  /**
   * returns P, the number of minhashes per signature
   */
  uint64_t getP() const;

  /**
   * returns Q, the number signatures per input set
   */
  uint64_t getQ() const;

protected:

  std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> params_;

  /**
   * Get the next value from one of the random number sequence with a provided x
   *
   * @param x     the x parameter from the equation above
   * @param index the index of the coefficient tupe to be used (valid values
   *              are 0..numberOfCoefficients() - 1)
   */
  uint64_t getNextValue(uint64_t x, size_t index);

};

}
#endif
