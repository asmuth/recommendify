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
#ifndef _RECOMMENDIFY_FINGERPRINT_H
#define _RECOMMENDIFY_FINGERPRINT_H

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <vector>

namespace recommendify {

class Fingerprint {
public:

  /**
   * Create a new fingerprint. Takes ownership of the data vector
   *
   * @param data The data as a list of uint64_ts
   */
  Fingerprint(std::vector<uint64_t>&& data) :
      data_(data) {}

  /**
   * Move a fingerprint.
   */
  Fingerprint(Fingerprint&& sig) :
      data_(std::move(sig.data_)) {}

  /**
   * Copy a fingerprint.
   */
  Fingerprint(const Fingerprint& sig) :
      data_(sig.data_) {}


  /**
   * Compare this fingerprint with another fingerprint. Returns true if both
   * fingerprints match and false of they do not match
   */
  bool compareTo(const Fingerprint& other) const {
    if (data_.size() != other.data_.size()) {
      return false;
    }

    for (int i = 0; i < data_.size(); ++i) {
      if (data_[i] != other.data_[i]) {
        return false;
      }
    }

    return true;
  }

  /**
   * Return a human readable representation of this fingerprint
   */
  std::string humanReadable() const {
    size_t len = data_.size() * 17 + 1;
    char*  buf = static_cast<char *>(alloca(len));
    size_t pos = 0;

    for (const uint64_t part : data_) {
      int ret = snprintf(buf + pos, len - pos - 1, "%"PRIx64"-", part);

      if (ret > 0) {
        pos += ret;
      }
    }

    return std::string(buf, pos - 1);
  }

protected:

  std::vector<uint64_t> data_;

};

}
#endif
