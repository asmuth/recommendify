/**
 * This file is part of the "recommendify" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _RECOMMENDIFY_LSH_H
#define _RECOMMENDIFY_LSH_H

namespace recommendify {

/**
 * Abstract LSH base class. Subclasses implement various locality sensitive
 * hashing schemes. Currently implemented subclasses:
 *
 *  - MinHash (http://en.wikipedia.org/wiki/MinHash)
 *
 */
class LSH {
public:

};

}
#endif
