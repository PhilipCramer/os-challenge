//
// Created by nth on 27-09-2023.
//
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "request.h"
#include "messages.h"

Body* createBody(char* body) {
    Body *newBody;
    uint64_t t;
    printf("Creating body\n");
    newBody = malloc(sizeof(Body));
    newBody->hash = malloc(SHA256_DIGEST_LENGTH+1);
    printf("Hash\n");

    memcpy(newBody->hash, body, SHA);

    printf("Start\n");
    memcpy(&t, body[32], sizeof(t));
    printf("Insreting");
    newBody->start = t;
    printf("End\n");

    memcpy(&t, body[PACKET_REQUEST_END_OFFSET], sizeof(uint64_t));
    newBody->end = t;
    newBody->priority = body[PACKET_REQUEST_PRIO_OFFSET];
    return newBody;
}