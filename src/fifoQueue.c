//
// Created by Sandie Petersen on 04/10/2023.
//

#include "fifoQueue.h"

fifo_t* initialize(int size) {
    fifo_t* queue = malloc(sizeof(fifo_t));
    request_t* ar = calloc(size, sizeof(request_t));
    queue->index = 0;
    queue->tail = 0;
    queue->size = size;
    queue->requests = ar;
    return queue;
}

void enqueue(int client_sock, fifo_t* queue) {
    request_t *request = malloc(sizeof(request));
    request->client_socket = client_sock;
    queue->requests[queue->tail] = *request;
    queue->tail = queue->tail + 1;
}


request_t dequeue(fifo_t* queue) {
    request_t current = queue->requests[queue->index];
    queue->requests[queue->index] = NULL;
    queue->index++;
    return current;
}


bool isEmpty(fifo_t* queue) {
    return queue->index == queue->tail;
}
