#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/resource.h>

#include "messages.h"
#include "hashFinder.h"
#include "priority_queue.h"

#define NUMBER_OF_THREADS 1

typedef struct {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  int64_t start;
  int64_t end;
  int64_t result;
  uint *client_socket;
} task_t;

typedef struct {
  int port;
  int *s_fd;
  prio_queue_t *task_queue;
} serv_param_t;


int term_flag = 1;

void * comm_server(void *server_params);
void * hash_calc(void * task_queue);

void sig_handler(int signum){
  printf("\nCaught signal %d\n", signum);
  term_flag = 0;
}

int main(int argc, char *argv[]){
  if(argc != 2) {
    printf("Please provide a Portnumber\n");
    exit(132);
  }
  int port_num = atoi(argv[1]);
  if (port_num < 1 || port_num > 65535){
    printf("Invalid port number.\n");
    exit(132);
  }

  /*/==================== DIRTY TRICKS ====================//   
  id_t pid = getpid();
  printf("Server started with pid: %d\n", pid);
  //if(setpriority(PRIO_PROCESS, pid, -30) == -1) printf("You have no power here...\n");
  if (getuid() != 0){
    int meh = system("sudo ./server.out 5003");
    printf("%d\n", meh);
    exit(1);
  } else if (getuid() == 0) {
    printf("!!!UNLIMITED POWER!!!\n");
    system("whoami");
  }

//==================== DIRTY TRICKS ====================/*/

  signal(SIGINT, sig_handler);

  pthread_t hash_threads[NUMBER_OF_THREADS];
  pthread_t comm_thread;
  prio_queue_t hash_queue;
  int sock_fd;
  serv_param_t *server_params = malloc(sizeof(serv_param_t));


  initialize_queue(&hash_queue);
  server_params->port = port_num;
  server_params->s_fd = &sock_fd;
  server_params->task_queue = &hash_queue;


  if(pthread_create(&comm_thread, NULL, &comm_server, server_params) != 0){
    perror("Failed to create server thread");   
  }
  for(int i = 0; i < NUMBER_OF_THREADS; i++){
    if(pthread_create(&hash_threads[i], NULL, &hash_calc, &hash_queue) != 0){
      perror("Failed to create workerthread");
    }
  }

  while(term_flag){
    sleep(2);
  }

  if(hash_queue.size != 0) printf("Queue is not empty!!!\n");
  shutdown(sock_fd, SHUT_RD);
  if(close(sock_fd) != 0) printf("Couldn't close socket\n");
  pthread_cond_broadcast(&(hash_queue.cond));

  if(pthread_join(comm_thread, NULL) != 0){
    perror("Failed to join server thread");   
  }
  for(int i = 0; i < NUMBER_OF_THREADS; i++){
    if(pthread_join(hash_threads[i], NULL) != 0){
      perror("Failed to join workerthread");
    }
  }
  printf("Joined threads!\n");


  if(destroy_queue(&hash_queue) != 0){
    printf("Can't destroy queue...\n");
  }
  free(server_params);
  return 0;
}


void * hash_calc(void * task_queue){
  prio_queue_t *prio_queue = (prio_queue_t *) task_queue;
  for(;term_flag;){
    task_t * task = (task_t *) dequeue(prio_queue);
    printf("%c %c", 0x08, 0x08);
    fflush(stdout);
    if(task != NULL){
      uint64_t result = findHash(task->hash, task->start, task->end);
      result = htobe64(result);
      if(send(*(task->client_socket), &result, PACKET_RESPONSE_SIZE, 0) != PACKET_RESPONSE_SIZE){
        printf("Couldn't send response to %d\n", *(task->client_socket));
      }
      close(*(task->client_socket));
      free(task->client_socket);
      free(task);

    } else {
      printf("Queue returned NULL\n");
      return 0;
    }
  }
  return 0;
}


void * comm_server(void *server_params){
  //int count = 1;
  int port_num = ((serv_param_t *) server_params)->port;
  int *socket_desc = ((serv_param_t*) server_params)->s_fd;
  prio_queue_t *task_queue = ((serv_param_t*) server_params)->task_queue;
  unsigned int client_size;
  unsigned int *client_sock;
  struct sockaddr_in server_addr, client_addr;
  unsigned char client_message[PACKET_REQUEST_SIZE];

  // Clean buffers:
  memset(client_message, '\0', sizeof(client_message));


  // Create socket:
  *socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if(*socket_desc < 0){
    printf("Error while creating socket\n");
    exit(1);
  }
  printf("Socket created successfully\n");

  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_num);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // Bind to the set port and IP:
  if(bind(*socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
    printf("Couldn't bind to the port\n");
    exit(1);
  }
  printf("Sucessfully bind port %d\n", port_num);

  // Listen for clients:
  if(listen(*socket_desc, 100) < 0){
    printf("Error while listening\n");
    exit(1);
  }
  printf("\nListening for incoming connections....%c[5m.\n%c[m\nCurrent queue: ", 0x1B, 0x1B);
  fflush(stdout);
  for(;term_flag;){
    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    client_sock = malloc(sizeof(unsigned int));
    *client_sock = accept(*socket_desc, (struct sockaddr*)&client_addr, &client_size);


    if (*client_sock < 0){
      printf("Can't accept\n");
      free(client_sock);
      break;
    }

    //printf("%d :Client connected at IP: %s and port: %i\n", count, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Receive client's message:
    if (recv(*client_sock, client_message, sizeof(client_message), 0) < 0){
      printf("Couldn't receive\n");
      free(client_sock);
      break;
    }
    task_t *recv_task = malloc(sizeof(task_t));
    int64_t start, end;
    memcpy(&(recv_task->hash), client_message, SHA256_DIGEST_LENGTH);
    memcpy(&start, &client_message[PACKET_REQUEST_START_OFFSET] ,sizeof(int64_t));
    memcpy(&end, &client_message[PACKET_REQUEST_END_OFFSET], sizeof(int64_t));
    recv_task->client_socket = client_sock;
    recv_task->start = htobe64(start);
    recv_task->end = htobe64(end);

    char priority;
    memcpy(&priority, &client_message[PACKET_REQUEST_PRIO_OFFSET], sizeof(char));
    if(enqueue(task_queue, (void *) recv_task, priority) != 0){
      printf("Enqueue Failed!!\n");
      free(client_sock);
      free(recv_task);
      break;
    }
    printf("O");
    fflush(stdout);
    //count++;
  }

  shutdown(*socket_desc, 2);
  close(*socket_desc);

  return 0;
}
