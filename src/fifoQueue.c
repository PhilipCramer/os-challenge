//
// Created by Sandie Petersen on 04/10/2023.
//

#include "fifoQueue.h"

fifo_t* initialize(int size) {
    fifo_t* queue = malloc(sizeof(fifo_t));
    request_t** ar = calloc(size, sizeof(request_t*));
    queue->head = 0;
    queue->tail = 0;
    queue->size = size;
    queue->requests = ar;
    return queue;
}

void enqueue(int client_sock, fifo_t* queue) {
    request_t *request = malloc(sizeof(request_t));
    request->client_socket = client_sock;
    queue->requests[queue->tail] = request;
    queue->tail++;
    if(queue->tail == queue->size) queue->tail = 0;
}


request_t* dequeue(fifo_t* queue) {
    request_t *current = queue->requests[queue->head];
    queue->requests[queue->head] = NULL;
    queue->head++;
    if(queue->head == queue->size) queue->head = 0;
    return current;
}


bool isEmpty(fifo_t* queue) {
    return queue->head == queue->tail;
}
