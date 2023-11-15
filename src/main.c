#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "bits/pthreadtypes.h"
#include "endian.h"
#include "messages.h"
#include "hashFinder.h"
#include "priority_queue.h"
#include "semaphore.h"
#include "cache.h"

typedef struct {
    int port_number;
    prio_queue_t *queue;
} params_t;

typedef struct {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    uint64_t start;
    uint64_t end;
    unsigned int client;
} task_t;

void *producer(void *parameters) {

    params_t *params = parameters;
    prio_queue_t *queue = params->queue;
    unsigned int socket_desc, client_size, client_sock;
    struct sockaddr_in server_addr, client_addr;
    unsigned char client_message[PACKET_REQUEST_SIZE];



    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0) {
        printf("Error while creating socket\n");
        exit(1);
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(params->port_number);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("Couldn't bind to the port\n");
        exit(1);
    }
    printf("Done with binding\n");

    // Listen for clients:
    if (listen(socket_desc, 128) < 0) {
        printf("Error while listening\n");
        exit(1);
    }
    printf("\nListening for incoming connections.....\n");
    for (;;) {

        // Accept an incoming connection:
        client_size = sizeof(client_addr);
        client_sock = accept(socket_desc, (struct sockaddr *) &client_addr, &client_size);

        if (client_sock < 0) {
            printf("Can't accept\n");
            exit(1);
        }
        // printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        if (recv(client_sock, client_message, sizeof(client_message), 0) != 49) {
            printf("Couldn't receive\n");
            exit(1);
        }

        //Parsing of received message
        task_t *received_task = malloc(sizeof(task_t));
        memcpy(&(received_task->hash), client_message, SHA256_DIGEST_LENGTH);
        memcpy(&(received_task->start), client_message + PACKET_REQUEST_START_OFFSET, sizeof(uint64_t));

        received_task->start = be64toh(received_task->start);

        memcpy(&(received_task->end), client_message + PACKET_REQUEST_END_OFFSET, sizeof(uint64_t));

        received_task->end = be64toh(received_task->end);

        memcpy(&(received_task->client), &client_sock, sizeof(unsigned int));
        uint64_t task_priority = (uint64_t) client_message[PACKET_REQUEST_PRIO_OFFSET];


        uint64_t cached_value = search(received_task->hash);

        if (cached_value > 0) {
            cached_value = htobe64(cached_value);
            if (send(client_sock, &cached_value, PACKET_RESPONSE_SIZE, 0) != PACKET_RESPONSE_SIZE) {
                printf("Can't send\n");
                exit(1);
            }
            // Closing the socket:
            close(client_sock);
            free(received_task);
        } else {

            enqueue(queue, (void *) received_task, task_priority);

        }
    }
}

void *consumer(void *parameter) {
    params_t *parameters = parameter;
    prio_queue_t *queue = parameters->queue;
    task_t *current_task;

    for (;;) {

        current_task = (task_t *) dequeue(queue);


        // Respond to client:
        uint64_t response = search(current_task->hash);

        if (response <= 0) {

            // Respond to client:
            response = find_hash(current_task->hash, current_task->start, current_task->end);

            insert(current_task->hash, response);

        }
        response = htobe64(response);
        if (send(current_task->client, &response, PACKET_RESPONSE_SIZE, 0) != PACKET_RESPONSE_SIZE) {
            printf("Can't send\n");
            exit(1);
        }

        // Closing the socket:
        close(current_task->client);
        free(current_task);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Please provide a Portnumber\n");
        exit(132);
    }

    int port_num = atoi(argv[1]);
    if (port_num < 1 || port_num > 65535) {
        printf("Invalid port number.\n");
        exit(132);
    }

    pthread_t consumer_thread;
    pthread_t producer_thread;


    prio_queue_t *queue = malloc(sizeof(prio_queue_t));
    initialize_queue(queue);


    params_t *param = malloc(sizeof(params_t));
    param->queue = queue;
    param->port_number = port_num;
    initialize_cache();

    pthread_create(&producer_thread, NULL, producer, (void *) param);
    pthread_create(&consumer_thread, NULL, consumer, (void *) param);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    free(param);
    destroy_queue(queue);
    free(queue);

    return 0;
}
