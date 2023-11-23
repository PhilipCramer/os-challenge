//
// Created by Sandie Petersen on 04/10/2023.
//

#include "fifoQueue.h"

extern int term_flag;

fifo_t *initialize(int size) {
    fifo_t *queue = malloc(sizeof(fifo_t));
    void **ar = calloc(size, sizeof(void *));
    queue->head = 0;
    queue->tail = 0;
    queue->size = size;
    queue->requests = ar;
    pthread_mutex_init(&(queue->queue_lock), NULL);
    pthread_cond_init(&(queue->queue_cond), NULL);
    return queue;
}

void enqueue(void *data, fifo_t *queue) {
    pthread_mutex_lock(&(queue->queue_lock));
    queue->requests[queue->tail] = data;
    queue->tail++;
    if (queue->tail == queue->size) queue->tail = 0;
    pthread_cond_signal(&(queue->queue_cond));
    pthread_mutex_unlock(&(queue->queue_lock));
}


void *dequeue(fifo_t *queue) {
    pthread_mutex_lock(&(queue->queue_lock));

    while (isEmpty(queue)) {
        pthread_cond_wait(&(queue->queue_cond), &(queue->queue_lock));
        if(term_flag) return NULL;
    }

    void *current = queue->requests[queue->head];
    queue->requests[queue->head] = NULL;
    queue->head++;
    if (queue->head == queue->size) queue->head = 0;
    pthread_mutex_unlock(&(queue->queue_lock));

    return current;
}


bool isEmpty(fifo_t *queue) {
    return queue->head == queue->tail;
}
