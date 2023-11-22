//
// Created by Sandie Petersen on 04/10/2023.
//

#ifndef OS_CHALLENGE_GROUP_08_FIFOQOUE_H
#define OS_CHALLENGE_GROUP_08_FIFOQOUE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    int head;
    int tail;
    int size;
    void** requests;
    pthread_mutex_t q_lock;
    pthread_cond_t queue_cond;
} fifo_t;

void enqueue(void * data, fifo_t* queue) ;
void* dequeue(fifo_t* queue);
bool isEmpty(fifo_t* queue);
fifo_t* initialize(int size);

#endif //OS_CHALLENGE_GROUP_08_FIFOQOUE_H
