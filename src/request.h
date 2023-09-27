//
// Created by nth on 27-09-2023.
//

#ifndef OS_CHALLENGE_GROUP_08_REQUEST_H
#define OS_CHALLENGE_GROUP_08_REQUEST_H
#include <stdint.h>
typedef struct body {
    uint64_t start;
    uint64_t end;
    char priority;
    uint8_t* hash;
} Body;

Body* createBody(char* requestBody);

#endif //OS_CHALLENGE_GROUP_08_REQUEST_H

