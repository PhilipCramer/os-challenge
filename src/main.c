#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "messages.h"
#include "hashFinder.h"
#include "fifoQueue.h"
#include "semaphore.h"

typedef struct {
    int port_number;
    fifo_t *queue;
    sem_t semaphore;
} params_t;

void *producer(void *parameters){
    params_t *params = parameters;
    fifo_t *queue = params->queue;
    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    unsigned char server_message[PACKET_RESPONSE_SIZE], client_message[PACKET_REQUEST_SIZE], recvHash[SHA256_DIGEST_LENGTH];



    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(params->port_number);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind to the set port and IP:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");

    // Listen for clients:
    if(listen(socket_desc, 128) < 0){
        printf("Error while listening\n");
        return -1;
    }
    printf("\nListening for incoming connections.....\n");
    do {
        // Accept an incoming connection:
        client_size = sizeof(client_addr);
        client_sock = accept(socket_desc, (struct sockaddr *) &client_addr, &client_size);

        if (client_sock < 0) {
            printf("Can't accept\n");
            return -1;
        }

//    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        enqueue(client_sock,queue);
    }while(1);
}

void* consumer(void * parameter){
    params_t* parameters = parameter;
    fifo_t *queue = parameters->queue;
    sem_t semaphore = parameters->semaphore;
    int client_sock;
    unsigned char server_message[PACKET_RESPONSE_SIZE], client_message[PACKET_REQUEST_SIZE], recvHash[SHA256_DIGEST_LENGTH];

    sem_wait(&semaphore);

    /*
    client_sock = dequeue(queue).clientSocket;
    if (recv(client_sock, client_message, sizeof(client_message), 0) < 0){
        printf("Couldn't receive\n");
        return -1;
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
     */

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
    pthread_t consumer;

    pthread_t producer;

    sem_t semaphore;

    fifo_t *queue = initialize(16);

    sem_init(&semaphore, 0, 0);

    params_t *param = malloc(sizeof(params_t));
    param->queue = queue;
    param->semaphore = semaphore;
    param->port_number = port_num;
    pthread_create(&producer,NULL,producer,(void *) param);
    pthread_create(&consumer,NULL,consumer,(void *) param);

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

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
