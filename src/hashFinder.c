#include "hashFinder.h"

int64_t findHash(char hashToFind[], int64_t start, int64_t stop) {
  char *foundHash;
  int64_t current;
  for(current = start; current < stop; current++){
    foundHash = SHA256((const unsigned char *) &current, sizeof(current), NULL);
    if (strcmp(hashToFind, foundHash) == 0){
      //free(foundHash);
      return current;
    }
  }
  return -1;
}
