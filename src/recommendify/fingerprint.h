/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _RECOMMENDIFY_FINGERPRINT_H
#define _RECOMMENDIFY_FINGERPRINT_H

#include <stdlib.h>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <vector>

namespace recommendify {

class Fingerprint {
public:

  /**
   * Create a new fingerprint.
   *
   * @param data The data as a list of uint64_ts
   */
  explicit Fingerprint(const std::vector<uint64_t>& data) :
      data_(data) {}

  /**
   * Create a new fingerprint. Takes ownership of the data vector
   *
   * @param data The data as a list of uint64_ts
   */
  explicit Fingerprint(std::vector<uint64_t>&& data) :
      data_(std::move(data)) {}

  /**
   * Move a fingerprint.
   */
  explicit Fingerprint(Fingerprint&& sig) :
      data_(std::move(sig.data_)) {}

  /**
   * Copy a fingerprint.
   */
  explicit Fingerprint(const Fingerprint& sig) :
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
    if (data_.size() == 0) {
      return std::string();
    }

    size_t len = data_.size() * 17 + 1;
    char*  buf = static_cast<char *>(alloca(len));
    size_t pos = 0;

    for (const uint64_t part : data_) {
      int ret = snprintf(buf + pos, len - pos - 1, "%lu-", part);

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
