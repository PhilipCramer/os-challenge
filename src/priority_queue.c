#include "priority_queue.h"

#define INITIAL_QUEUE_SIZE 128


int initialize_queue(prio_queue_t *reference){
  qelement_t *arr = calloc(INITIAL_QUEUE_SIZE, sizeof(qelement_t));
  if (arr == NULL) return 1;
  reference->array = arr;
  reference->size = 0;
  reference->last = INITIAL_QUEUE_SIZE;
  return 0;
}
int enqueue(prio_queue_t *reference, void *data, int priority){
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
  return 0;
}
void bubbleUp(prio_queue_t *reference, int pos){
  int lookup = pos/2;
  int current = pos;
  qelement_t *array = reference->array;
  while(array[current].priority < array[lookup].priority && current > 1){
    qelement_t temp = array[lookup];
    array[lookup] = array[current];
    array[current] = temp;
    current = lookup;
    lookup = current/2;
  }
}
void *dequeue(prio_queue_t *reference){
  qelement_t *array = reference->array;
  void *res = array[1].element;
  array[1] = array[reference->size];
  reference->size--;

  if(reference->size == 0){
    qelement_t * new_array = realloc(reference->array, INITIAL_QUEUE_SIZE * sizeof(qelement_t));
    if(new_array != NULL){
      reference->array = new_array;
      reference->last /= 2;
    } else return NULL;
  }

  bubbleDown(reference, 1);

  return res;
}

void bubbleDown(prio_queue_t *reference, int pos){
  qelement_t *array = reference->array;
  int look = pos*2;
  int current = pos;
  qelement_t temp;
  while(look <= reference->size){
    if(array[look].priority < array[look+1].priority){
      look += 1;
    }
    if(array[current].priority < array[look].priority){
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
    return 0;
  }else return 1;
}
