//
// Created by Sandie Petersen on 04/10/2023.
//

#include "fifoQueue.h"

fifo* initialize(int size) {
    fifo* queue = malloc(sizeof(fifo));
    request* ar = calloc(size, sizeof(request));
    queue->index = 0;
    queue->tail = 0;
    queue->size = size;
    queue->requests = ar;
    return queue;
}

void enqueue(int client_socket, fifo* queue) {
    request *request = malloc(sizeof(request));
    request->clientSocket = client;
    queue->requests[queue->tail] = *request;
    queue->tail = queue->tail + 1;
}


request dequeue(fifo* queue) {
    request current = queue->requests[queue->index];
    queue->requests[queue->index] = NULL;
    queue->index++;
    return current;
}


bool isEmpty(fifo* queue) {
    return queue->index == queue->tail;
}