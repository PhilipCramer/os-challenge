#include "priority_queue.h"

#define COMPARE <
#define INITIAL_QUEUE_SIZE 16

int initialize_queue(prio_queue_t *reference){
  qelement_t *arr = calloc(INITIAL_QUEUE_SIZE, sizeof(qelement_t));
  if (arr == NULL) return 1;
  reference->array = arr;
  reference->size = 0;
  pthread_mutex_init(&(reference->lock), NULL); 
  pthread_cond_init(&(reference->cond), NULL);
  reference->last = INITIAL_QUEUE_SIZE;
  return 0;
}


int enqueue(prio_queue_t *reference, void *data, int prio_level){
  pthread_mutex_lock(&(reference->lock));
  /*if(reference->size == 0){
    clock_gettime(CLOCK_TAI, &(reference->queue_time));
  }
  struct timespec enqueue_time;
  clock_gettime(CLOCK_TAI, &enqueue_time);
  long priority = ((enqueue_time.tv_sec - (reference->queue_time).tv_sec) * 1000000 + (enqueue_time.tv_nsec - (reference->queue_time).tv_nsec) / 1000) / prio_level;*/
  if(reference->size == reference->last - 1){
    qelement_t * new_array = realloc(reference->array, sizeof(qelement_t) * 2 * reference->last);
    if(new_array != NULL){
      reference->array = new_array;
      reference->last *= 2;
    } else {
      printf("Couldn't expand array.\n");
      //pthread_mutex_unlock(&(reference->mutex));
      return 1;
    }
  }
  reference->size++;
  reference->array[reference->size].data = data;
  reference->array[reference->size].priority = prio_level;
  bubbleUp(reference,reference->size);
  pthread_cond_signal(&(reference->cond));
  pthread_mutex_unlock(&(reference->lock));
  return 0;
}


void bubbleUp(prio_queue_t *reference, int pos){
  int lookup = pos/2;
  int current = pos;
  qelement_t *array = reference->array;
  while(array[lookup].priority COMPARE array[current].priority && current > 1){
    qelement_t temp = array[lookup];
    array[lookup] = array[current];
    array[current] = temp;
    current = lookup;
    lookup = current/2;
  }
}


void *dequeue(prio_queue_t *reference){
  pthread_mutex_lock(&(reference->lock));

  if (reference->size == 0){
    //pthread_mutex_unlock(&(reference->mutex));
    if(pthread_cond_wait(&(reference->cond), &(reference->lock)) != 0) 
      return NULL;
    if (reference->size == 0) return NULL;
  }

  if((reference->size == reference->last / 4) && (reference->last / 4 > INITIAL_QUEUE_SIZE)){
    qelement_t * new_array = realloc(reference->array, (reference->last / 2) * sizeof(qelement_t));
    if(new_array != NULL){
      reference->array = new_array;
      reference->last /= 2;
    } else {
      printf("Error reducing queue size!\n");
      pthread_mutex_unlock(&(reference->lock));
      return NULL;
    }
  }

  void *res = reference->array[1].data;
  reference->array[1] = reference->array[reference->size];
  reference->size--;

  bubbleDown(reference, 1);
  pthread_mutex_unlock(&(reference->lock));

  return res;
}

void bubbleDown(prio_queue_t *reference, int pos){
  qelement_t *array = reference->array;
  int look = pos*2;
  int current = pos;
  qelement_t temp;
  while(look <= reference->size){
    if(array[look +1 ].priority COMPARE array[look+1].priority){
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
  /*if(pthread_mutex_trylock(&(reference->lock)) != 0){
    printf("Queue is locked...\n");
    return 1;
  }else*/ if (reference->size != 0){
    printf("Queue isn't empty...\n");
    return 1;
  }else{
    free(reference->array);
    pthread_cond_destroy(&(reference->cond));
    pthread_mutex_unlock(&(reference->lock));
    pthread_mutex_destroy(&(reference->lock));
    return 0;
  }
}
