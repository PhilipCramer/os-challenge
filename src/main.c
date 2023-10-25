#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "bits/pthreadtypes.h"
#include "messages.h"
#include "hashFinder.h"
#include "fifoQueue.h"
#include "semaphore.h"

typedef struct {
    int port_number;
    fifo_t *queue;
    pthread_cond_t queue_cond;
    pthread_mutex_t queue_lock; 
} params_t;

typedef struct {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    uint64_t start;
    uint64_t end;
    unsigned int client;
} task_t;

void *producer(void *parameters){
    params_t *params = parameters;
    fifo_t *queue = params->queue;
    unsigned int socket_desc, client_size, client_sock;
    struct sockaddr_in server_addr, client_addr;
    unsigned char client_message[PACKET_REQUEST_SIZE];



    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_desc < 0){
        printf("Error while creating socket\n");
        exit(1);
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(params->port_number);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind to the set port and IP:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Couldn't bind to the port\n");
        exit(1);
    }
    printf("Done with binding\n");

    // Listen for clients:
    if(listen(socket_desc, 128) < 0){
        printf("Error while listening\n");
        exit(1);
    }
    printf("\nListening for incoming connections.....\n");
    for(;;){
        // Accept an incoming connection:
        client_size = sizeof(client_addr);
        client_sock = accept(socket_desc, (struct sockaddr *) &client_addr, &client_size);

        if (client_sock < 0) {
            printf("Can't accept\n");
            exit(1);
        }
        printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        if (recv(client_sock, client_message, sizeof(client_message), 0) < 0){
            printf("Couldn't receive\n");
            exit(1);
        }

        //Parsing of received message
        task_t * received_task = malloc(sizeof(task_t));
        memcpy(&(received_task->hash), client_message, SHA256_DIGEST_LENGTH);
        memcpy(&(received_task->start), client_message + PACKET_REQUEST_START_OFFSET, sizeof(uint64_t));
        memcpy(&(received_task->end), client_message + PACKET_REQUEST_END_OFFSET, sizeof(uint64_t));
        memcpy(&(received_task->client), &client_sock, sizeof(unsigned int));

        pthread_mutex_lock(&(params->queue_lock));
        enqueue((void *) received_task, queue);
        pthread_mutex_unlock(&(params->queue_lock));
        pthread_cond_signal(&(params->queue_cond));
    }
}

void* consumer(void * parameter){
    params_t* parameters = parameter;
    fifo_t *queue = parameters->queue;
    task_t* current_task;
    unsigned char server_message[PACKET_RESPONSE_SIZE];

    for(;;){
      pthread_mutex_lock(&(parameters->queue_lock));
      if(isEmpty(queue)) pthread_cond_wait(&(parameters->queue_cond), &(parameters->queue_lock));
      current_task = (task_t *) dequeue(queue);
      pthread_mutex_unlock(&(parameters->queue_lock));

    // Respond to client:
    uint64_t response = find_hash(current_task->hash, be64toh(current_task->start), be64toh(current_task->end));

    response = htobe64(response);

    //memcpy(server_message, &response, PACKET_RESPONSE_SIZE);

    if (send(current_task->client, &response, PACKET_RESPONSE_SIZE, 0) != PACKET_RESPONSE_SIZE){
        printf("Can't send\n");
        exit(1);
    }

    // Closing the socket:
    close(current_task->client);
    free(current_task);
    
    }

}

int main(int argc, char *argv[]){
    if(argc != 2) {
      printf("Please provide a Portnumber\n");
      exit(132);
    }

    int port_num =atoi(argv[1]);
    if (port_num < 1 || port_num > 65535){
      printf("Invalid pport number.\n");
      exit(132);
    }
    pthread_t consumer_thread;
    pthread_t producer_thread;


    fifo_t *queue = initialize(1000);


    params_t *param = malloc(sizeof(params_t));
    param->queue = queue;
    param->port_number = port_num;
    pthread_cond_init(&(param->queue_cond), NULL);
    pthread_mutex_init(&(param->queue_lock), NULL);


    pthread_create(&producer_thread,NULL,producer,(void *) param);
    pthread_create(&consumer_thread,NULL,consumer,(void *) param);
    
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

   return 0;
}

void oldCode() {
    /*
    if(argc != 2) {
      printf("Please provide a Portnumber\n");
      exit(132);
    }

    int port_num =atoi(argv[1]);
    if (port_num < 1 || port_num > 65535){
      printf("Invalid pport number.\n");
      exit(132);
    }


    // Respond to client:
    int64_t start, end;
    memcpy(recvHash,client_message,SHA256_DIGEST_LENGTH);
    memcpy(&start,client_message + PACKET_REQUEST_START_OFFSET,sizeof(uint64_t));
    memcpy(&end,client_message + PACKET_REQUEST_END_OFFSET,sizeof(uint64_t));
    int64_t response = findHash(recvHash, be64toh(start), be64toh(end));

    response = htobe64(response);

    memcpy(server_message, &response, PACKET_RESPONSE_SIZE);

    if (send(client_sock, server_message, PACKET_RESPONSE_SIZE, 0) != PACKET_RESPONSE_SIZE){
        printf("Can't send\n");
        return -1;
    }


    // Closing the socket:
    close(client_sock);
    shutdown(socket_desc, 2);
    close(socket_desc);
    */

}
