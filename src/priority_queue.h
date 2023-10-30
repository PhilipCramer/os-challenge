#include<stdlib.h>
#include <pthread.h>

typedef struct {
  void * element;
  int priority;
} qelement_t;

typedef struct {
  int size;
  int last;
  qelement_t *array;
}prio_queue_t;

int initialize_queue(prio_queue_t *reference); 
int enqueue(prio_queue_t *reference, void *data, int priority);
void bubbleUp(prio_queue_t *reference, int pos);
void bubbleDown(prio_queue_t *reference, int pos);
void *dequeue(prio_queue_t *reference);
int destroy_queue(prio_queue_t * reference);
void printQ(prio_queue_t *reference);

