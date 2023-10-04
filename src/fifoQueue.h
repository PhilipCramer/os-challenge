//
// Created by Sandie Petersen on 04/10/2023.
//

#ifndef OS_CHALLENGE_GROUP_08_FIFOQOUE_H
#define OS_CHALLENGE_GROUP_08_FIFOQOUE_H
enqueue(request* conection, fifo* queue) ;
void* dequeue(fifo* queue);
bool isEmplty(fifo* queue);

typedef struct request {
    int clientSocket;
}request;

typedef struct fifo {
    int index;
    int tail;
    int size;
    request* requests;
}fifo;


#endif //OS_CHALLENGE_GROUP_08_FIFOQOUE_H
