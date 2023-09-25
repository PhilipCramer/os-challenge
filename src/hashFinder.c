#include "hashFinder.h"

int64_t findHash(unsigned char hashToFind[], int64_t start, int64_t stop) {
  unsigned char *foundHash;
  int64_t current;
  for(current = start; current < stop; current++){
    foundHash = SHA256((const unsigned char *) &current, sizeof(current), NULL);
    if (memcmp(hashToFind, foundHash, SHA256_DIGEST_LENGTH) == 0){
      return current;
    }
  }
  return -1;
}
