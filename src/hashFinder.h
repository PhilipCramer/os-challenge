#include <openssl/sha.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint64_t find_hash(unsigned char hash_to_find[], uint64_t start, uint64_t stop);
