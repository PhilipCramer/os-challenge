//
// Created by Sandie Petersen on 04/10/2023.
//

#include "fifoQueue.h"

fifo_t* initialize(int size) {
    fifo_t* queue = malloc(sizeof(fifo_t));
    void** ar = calloc(size, sizeof(void*));
    queue->head = 0;
    queue->tail = 0;
    queue->size = size;
    queue->requests = ar;
    return queue;
}

void enqueue(void * data, fifo_t* queue) {
    queue->requests[queue->tail] = data;
    queue->tail++;
    if(queue->tail == queue->size) queue->tail = 0;
}


void * dequeue(fifo_t* queue) {
    void * current = queue->requests[queue->head];
    queue->requests[queue->head] = NULL;
    queue->head++;
    if(queue->head == queue->size) queue->head = 0;
    return current;
}


bool isEmpty(fifo_t* queue) {
    return queue->head == queue->tail;
}
