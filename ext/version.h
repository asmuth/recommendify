#ifndef VERSION_H
#define VERSION_H

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_MICRO 1

#define VERSION_STRING "recommendify_native %i.%i.%i\n" \
 "\n" \
 "Copyright © 2012\n" \
 "  Paul Asmuth <paul@paulasmuth.com>\n" 

#define USAGE_STRING "usage: %s " \
 "{--version|--jaccard|--cosine} " \
 "[redis_key] [item_id]\n"

#endif