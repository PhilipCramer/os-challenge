#include "hashFinder.h"

uint64_t find_hash(unsigned char hash_to_find[], uint64_t start, uint64_t stop) {
  unsigned char *found_hash;
  uint64_t current;
  unsigned char buffer[32];
  for(current = start; current < stop; current++){
    found_hash = SHA256((const unsigned char *) &current, sizeof(current), buffer);
    if (memcmp(hash_to_find, buffer, SHA256_DIGEST_LENGTH) == 0){
      return current;
    }
  }
  return -1;
}
