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
#include <vector>
#include <random>
#include <set>
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
void printUserRecos(const ItemSet& query_point);

/**
 * Our "database"
 */
static std::unordered_map<uint64_t, std::string> movie_names_by_id;
static std::unordered_map<uint64_t, std::set<uint64_t>> preference_sets;
static std::vector<uint64_t> user_ids;

int main() {
  printf("Loading movies.dat...\n");
  /* Input file movies.dat has the format (movie_id,movie_name,movie_tags) */
  readDatFile("./movies.dat", [] (const std::vector<std::string>& line) {
    movie_names_by_id[atoi(line[0].c_str())] = line[1];
  });

  printf("Loading ratings.dat...\n");
  /* Input file ratings.dat has the format (user_id,movie_id,rating,time). */
  readDatFile("./ratings.dat", [] (const std::vector<std::string>& line) {
    /* Construct binary ratings / preference sets by only considering votes with
       a score greater than or equal to 3 */
    if (atoi(line[2].c_str()) < 3) {
      return;
    }

    uint64_t user_id = atoi(line[0].c_str());

    if (preference_sets.find(user_id) == preference_sets.end()) {
      user_ids.push_back(user_id);
    }

    preference_sets[user_id].insert(atoi(line[1].c_str()));
  });

  /* Set up the minhash processor with random parameters */
  uint64_t p = 4, q = 10;
  std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> minhash_params;
  MinHash::generateParameters(minhash_params, p * q);

  MinHashRecommender recommender(
    MinHash(p, q, minhash_params),
    MemoryBackend());

  /* Add all preference sets to the recommender */
  printf("Importing preference sets...\n");
  for (const auto& pset : preference_sets) {
    ItemSet pset_is(std::vector<uint64_t>(
        pset.second.begin(), pset.second.end()));

    recommender.addPreferenceSet(pset_is);
  }

  printf("Recommender ready :)\n");

  /* Sample some random users and print their input sets and recommendations */
  std::mt19937 prng((std::random_device())());

  for (int i = 0; i < 10;) {
    std::uniform_int_distribution<> dist(0, user_ids.size() - 1);
    uint64_t user_id = dist(prng);
    const auto pset = preference_sets[user_ids[user_id]];

    if (pset.size() < 10) {
      printUserRecos(ItemSet(std::vector<uint64_t>(pset.begin(), pset.end())));
      printf("%lu\n", user_id);
      user_ids.erase(std::find(user_ids.begin(), user_ids.end(), user_id));
      ++i;
    }
  }
}

void printUserRecos(const ItemSet& query_point) {
  printf("==== Recommendation ===\n");
  printf("Input Set:");
  for (const uint64_t item : query_point.getItems()) {
    printf("%s, ", movie_names_by_id[item].c_str());
  }
  printf("\n");
}

/**
 * Helper code to read the .dat/csv files
 */
#define CSV_READ_BUF_SIZE 4096
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

