//
// Created by Sandie Petersen on 04/10/2023.
//

#ifndef OS_CHALLENGE_GROUP_08_FIFOQOUE_H
#define OS_CHALLENGE_GROUP_08_FIFOQOUE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct request {
    int clientSocket;
}request;

typedef struct fifo {
    int index;
    int tail;
    int size;
    request* requests;
}fifo;

void enqueue(int client_socket, fifo* queue) ;
request dequeue(fifo* queue);
bool isEmpty(fifo* queue);
fifo* initialize(int size);

#endif //OS_CHALLENGE_GROUP_08_FIFOQOUE_H
