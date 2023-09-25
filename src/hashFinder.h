#include <openssl/sha.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int64_t findHash(unsigned char hashToFind[], int64_t start, int64_t stop);
