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
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>
#include "recommendify/minhashrecommender.h"

/**
 * A minhash/personalized recommender based on the open GroupLens MovieLens 1M
 * data set. You need {movies,ratings,users}.dat which can be downloaded here:
 *
 *   http://grouplens.org/datasets/movielens/
 *
 */
using namespace recommendify;
void readDatFile(const char* filename,
    std::function<void(const std::vector<std::string>&)> callback);

/**
 * Our "database"
 */
static std::unordered_map<uint64_t, std::string> movie_names_by_id;

int main() {
  readDatFile("./movies.dat", [] (const std::vector<std::string>& line) {
    movie_names_by_id[atoi(line[0].c_str())] = line[1];
  });
}


/**
 * Helper code to read the .dat/csv files
 */
#define CSV_READ_BUF_SIZE 1024
void readDatFile(const char* filename,
    std::function<void(const std::vector<std::string>&)> callback) {
  std::vector<std::string> line;
  char buf[CSV_READ_BUF_SIZE];
  size_t pos = 0;
  int bytes;
  int fd = open(filename, O_RDONLY);

  if (fd < 1) {
    perror("open() failed\n");
    return;
  }

  do {
    bytes = read(fd, buf + pos, CSV_READ_BUF_SIZE - pos);

    if (bytes > 0) {
      pos += bytes;
    }

    size_t last = 0;

    for (size_t cur = 0; cur < pos; ++cur) {
      bool is_separator = (cur > 0 && buf[cur] == ':' && buf[cur - 1] == ':');

      if (buf[cur] == '\n' ||  is_separator) {
        line.push_back(std::string(buf + last, cur - last - (
            is_separator ? 1 : 0)));

        last = cur + 1;

        if (buf[cur] == '\n') {
          callback(line);
          line.clear();
        }
      }
    }

    if (last > 0) {
      memcpy(buf, buf + last, pos - last);
      pos -= last;
    }

  } while (bytes > 0);

  close(fd);
}

