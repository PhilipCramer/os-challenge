#include "hashFinder.h"

uint64_t findHash(unsigned char hashToFind[], int64_t start, int64_t stop) {
  unsigned char foundHash[SHA256_DIGEST_LENGTH];
  int64_t current;
  for(current = start; current <= stop; current++){
    SHA256((const unsigned char *) &current, sizeof(current), foundHash);
    if (memcmp(hashToFind, foundHash, SHA256_DIGEST_LENGTH) == 0){
      return current;
    }
  }
  return 0;
}
