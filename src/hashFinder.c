#include "hashFinder.h"

uint64_t find_hash(unsigned char hash_to_find[], uint64_t start, uint64_t stop) {
  unsigned char *found_hash;
  uint64_t current;
  for(current = start; current < stop; current++){
    found_hash = SHA256((const unsigned char *) &current, sizeof(current), NULL);
    if (memcmp(hash_to_find, found_hash, SHA256_DIGEST_LENGTH) == 0){
      return current;
    }
  }
  return -1;
}
