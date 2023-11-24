//
// Created by Sandie Petersen on 16/11/2023.
//

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "bits/pthreadtypes.h"
#include "semaphore.h"
#include <stdlib.h>
#include "stdbool.h"


#ifndef OS_CHALLENGE_GROUP_08_CALCULATEHASH_H
#define OS_CHALLENGE_GROUP_08_CALCULATEHASH_H

typedef struct {
    uint64_t start;
    uint64_t end;
    unsigned char *hash;
    uint64_t* response;
    bool keep_calculating;
    pthread_mutex_t *lock;
    pthread_cond_t *done;
} calculation_params_t;

void* calculate_hash(calculation_params_t * param);

#endif //OS_CHALLENGE_GROUP_08_CALCULATEHASH_H