#include "priority_queue.h"
#include "pthread.h"
#include <stdint.h>

#define INITIAL_QUEUE_SIZE 128
#define COMPARE <


int initialize_queue(prio_queue_t *reference){
  qelement_t *arr = calloc(INITIAL_QUEUE_SIZE, sizeof(qelement_t));
  if (arr == NULL) return 1;
  reference->array = arr;
  reference->size = 0;
  reference->last = INITIAL_QUEUE_SIZE;
  pthread_mutex_init(&reference->queue_lock, NULL);
  pthread_cond_init(&reference->queue_cond, NULL);
  return 0;
}
int enqueue(prio_queue_t *reference, void *data, int priority){
  pthread_mutex_lock(&(reference->queue_lock));
  if(reference->size == reference->last - 1){
    qelement_t * new_array = realloc(reference->array,reference->last * sizeof(qelement_t) * 2);
    if(new_array != NULL){
      reference->array = new_array;
      reference->last *= 2;
    } else return 1; 
  }
  reference->size++;
  reference->array[reference->size].element = data;
  reference->array[reference->size].priority = priority;
  bubbleUp(reference,reference->size);
  pthread_cond_signal(&(reference->queue_cond));
  pthread_mutex_unlock(&(reference->queue_lock));
  return 0;
}
void bubbleUp(prio_queue_t *reference, int pos){
  int lookup = pos/2;
  int current = pos;
  qelement_t temp;
  qelement_t *array = reference->array;
  while(array[lookup].priority COMPARE array[current].priority && current > 1){
    temp = array[lookup];
    array[lookup] = array[current];
    array[current] = temp;
    current = lookup;
    lookup = current/2;
  }
}
void *dequeue(prio_queue_t *reference){
  pthread_mutex_lock(&(reference->queue_lock));
  if(reference->size == 0) {
    pthread_cond_wait(&(reference->queue_cond), &(reference->queue_lock));
    if(reference->size == 0){
      pthread_mutex_unlock(&(reference->queue_lock));
      return NULL;
    }
  }

  qelement_t *array = reference->array;
  void *res = array[1].element;
  array[1] = array[reference->size];
  reference->size--;

  if(reference->size == 0 && reference->last > INITIAL_QUEUE_SIZE){
    qelement_t * new_array = realloc(reference->array, INITIAL_QUEUE_SIZE * sizeof(qelement_t));
    if(new_array != NULL){
      reference->array = new_array;
      reference->last = INITIAL_QUEUE_SIZE;
    } else return NULL;
  }

  bubbleDown(reference, 1);
  pthread_mutex_unlock(&reference->queue_lock);
  return res;
}

void bubbleDown(prio_queue_t *reference, int pos){
  qelement_t *array = reference->array;
  int look = pos*2;
  int current = pos;
  qelement_t temp;
  while(look <= reference->size){
    if(array[look].priority COMPARE array[look+1].priority){
      look += 1;
    }
    if(array[current].priority COMPARE array[look].priority){
      temp = array[current];
      array[current] = array[look];
      array[look] = temp;
    } else break;
    current = look;
    look = current * 2;
  }
}

int destroy_queue(prio_queue_t * reference){
  if(reference->size == 0){
    free(reference->array);
    pthread_mutex_destroy(&reference->queue_lock);
    pthread_cond_destroy(&reference->queue_cond);
    return 0;
  }else return 1;
}
