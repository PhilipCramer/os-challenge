//
// Created by Sandie Petersen on 04/10/2023.
//

#ifndef OS_CHALLENGE_GROUP_08_FIFOQOUE_H
#define OS_CHALLENGE_GROUP_08_FIFOQOUE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    int client_socket;
} request_t;

typedef struct {
    int head;
    int tail;
    int size;
    request_t** requests;
} fifo_t;

void enqueue(int client_socket, fifo_t* queue) ;
request_t* dequeue(fifo_t* queue);
bool isEmpty(fifo_t* queue);
fifo_t* initialize(int size);

#endif //OS_CHALLENGE_GROUP_08_FIFOQOUE_H
