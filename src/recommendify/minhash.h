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
#include <stdint.h>
#include <vector>
#include <string>
#include "lsh.h"

namespace recommendify {

class MinHash : public LSH {

public:

  /**
   * Create a new MinHash processor.
   *
   * The two params p and q control how many minhashes will be calculated for
   * each signature and how many signatures will be calculated per input set
   * respectively. The total number of computed hashes will be p * q.
   *
   * Good values to start experimenting are:
   *   p = {1..4}
   *   q = {1..100}
   *
   * @param p The number of individual minhashes that make up a single hash
   *          signature
   * @param q The number of hash signatures to calculate per input set
   */
  MinHash(uint64_t p, uint64_t q);
  MinHash(MinHash& copy) = delete;

  /**
   * Compute a list of hash signatures for the input item set and store them in
   * the destination vector
   */
  void computeHashSignatures(
      const std::vector<uint64_t>& input_set,
      std::vector<std::string>& dest) const;

};

}
