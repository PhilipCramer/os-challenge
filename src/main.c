#include <signal.h>
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

//
typedef struct {
    int port_number;
    fifo_t *queue;
    unsigned int socket_desc;
} params_t;

typedef struct {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    uint64_t start;
    uint64_t end;
    unsigned int client;
} task_t;

int term_flag = 0;

void sig_handler(int signum){
  printf("\nCaught signal %d\n", signum);
  term_flag = !term_flag;
}

void *producer(void *parameters){
    params_t *params = parameters;
    fifo_t *queue = params->queue;
    unsigned int client_size, client_sock;
    struct sockaddr_in server_addr, client_addr;
    unsigned char client_message[PACKET_REQUEST_SIZE];



    // Create socket:
    params->socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if(params->socket_desc < 0){
        printf("Error while creating socket\n");
        term_flag = 1;
        return NULL;
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(params->port_number);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind to the set port and IP:
    if(bind(params->socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Couldn't bind to the port\n");
        term_flag = 1;
        return NULL;
    }
    printf("Done with binding\n");

    // Listen for clients:
    if(listen(params->socket_desc, 128) < 0){
        printf("Error while listening\n");
        term_flag = 1;
        return NULL;;
    }
    printf("\nListening for incoming connections.....\n");

    while(!term_flag){
        // Accept an incoming connection:
        client_size = sizeof(client_addr);
        client_sock = accept(params->socket_desc, (struct sockaddr *) &client_addr, &client_size);

        if (client_sock < 0) {
            printf("Can't accept\n");
            term_flag = 1;
            return NULL;
        }
        printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        if (recv(client_sock, client_message, sizeof(client_message), 0) < 0){
            printf("Couldn't receive\n");
            term_flag = 1;
            return NULL;
        }

        //Parsing of received message
        task_t * received_task = malloc(sizeof(task_t));
        memcpy(&(received_task->hash), client_message, SHA256_DIGEST_LENGTH);
        memcpy(&(received_task->start), client_message + PACKET_REQUEST_START_OFFSET, sizeof(uint64_t));
        memcpy(&(received_task->end), client_message + PACKET_REQUEST_END_OFFSET, sizeof(uint64_t));
        memcpy(&(received_task->client), &client_sock, sizeof(unsigned int));

        enqueue((void *) received_task, queue);

    }
  return NULL;
}

void* consumer(void * parameter){
    params_t* parameters = parameter;
    fifo_t *queue = parameters->queue;
    task_t* current_task;

    while(!term_flag){
      current_task = (task_t *) dequeue(queue);

      if(!current_task) 
          return NULL;
    

      // Respond to client:
      uint64_t response = find_hash(current_task->hash, be64toh(current_task->start), be64toh(current_task->end));

      response = htobe64(response);

      if (send(current_task->client, &response, PACKET_RESPONSE_SIZE, 0) != PACKET_RESPONSE_SIZE){
        printf("Can't send\n");
        term_flag = 1;
        return NULL;
        pthread_exit(NULL);
      }

      // Closing the socket:
      close(current_task->client);
      free(current_task); 
    }
  return NULL;
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

    signal(SIGINT,sig_handler);

    pthread_create(&producer_thread,NULL,producer,(void *) param);
    pthread_create(&consumer_thread,NULL,consumer,(void *) param);

    while(!term_flag){
      sleep(2);
    }
    printf("Beginning close procedure\n");
    if(shutdown(param->socket_desc, SHUT_RD) == 0){
      printf("Closed Receiving socket\n");
    }
    close(param->socket_desc);
    while(!isEmpty(queue) && term_flag){
      printf("Queue not empty... \nPress \"CTRL + C to abort without finishing\n");
      sleep(5);
    }
    
    while(!isEmpty(queue)){
      task_t *tmp = dequeue(queue);
      close(tmp->client);    
      free(tmp);
    }
    
    printf("Joining Producer thread\n") ;
    pthread_join(producer_thread, NULL);
    printf("Signaling\n");
    pthread_cond_signal(&param->queue->queue_cond);
    printf("Joining Consumer thread\n");
    pthread_join(consumer_thread, NULL);
    printf("Consumer joined");
    pthread_cond_destroy(&param->queue->queue_cond);
    pthread_mutex_destroy(&param->queue->queue_lock);

    free(param);
    free(queue->requests);
    free(queue);

   return 0;
}
