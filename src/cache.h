//
// Created by nts96 on 01/11/2023.
//

#ifndef OS_GITLAB_CACHE_H
#define OS_GITLAB_CACHE_H

#include "messages.h"
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CACHESIZE 300007

typedef struct element {
    unsigned char key[32];
    uint64_t value;
    struct element *next;
} Element;

void insert(unsigned char key[SHA256_DIGEST_LENGTH], uint64_t value);

uint64_t search(unsigned char* searchKey);

uint64_t calculateIndex(unsigned char* searchKey);

void initialize_cache();


#endif //OS_GITLAB_CACHE_H
