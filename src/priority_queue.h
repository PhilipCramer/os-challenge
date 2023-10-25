#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

typedef struct queue_element {
  void * data;
  long priority;
} qelement_t;

typedef struct priority_queue {
  pthread_mutex_t lock;
  pthread_cond_t cond;
  int size;
  int last;
  //struct timespec queue_time;
  qelement_t *array;
} prio_queue_t;

int initialize_queue(prio_queue_t *reference); 
int enqueue(prio_queue_t *reference, void *data, int priority);
void bubbleUp(prio_queue_t *reference, int pos);
void bubbleDown(prio_queue_t *reference, int pos);
void *dequeue(prio_queue_t *reference);
int destroy_queue(prio_queue_t * reference);

