#include "branch_prediction.h"
#include <stdio.h>
#include <time.h>

void set_enviroment(int env_value){
  srand(env_value);
}

unsigned int predict_enviroment(uint64_t point_start ,int difficulty){


  int start_random = 1; 
  int seed;
  uint64_t test_start;

  //printf("trying time seed\n");
  int time_seed = time((time_t*) 0x0);

  for(seed = time_seed + 100; seed > time_seed - 1100; seed--){
    srand(seed);

    uint tmp;
    if (start_random == 1) {
      tmp = rand();
      test_start = (long)tmp << 0x20;

      tmp = rand();
      test_start = (test_start | (long)tmp) % (-difficulty - 2);
    } else{
      test_start = 1;
    }
    tmp = rand();
    tmp = rand();
    //test_value = test_start + (ulong)(long)iVar3 % difficulty;

    if (/*test_value == point_val && */ test_start == point_start){
      return seed;
    }
  }
  //uint64_t test_value;
  for(seed = 0; seed <= 100000000; seed++){

    srand(seed);

    uint tmp;
    if (start_random == 1) {
      tmp = rand();
      test_start = (long)tmp << 0x20;

      tmp = rand();
      test_start = (test_start | (long)tmp) % (-difficulty - 2);
    }
    else{
      test_start = 1;
    }
    tmp = rand();
    tmp = rand();
    //test_value = test_start + (ulong)(long)iVar3 % difficulty;

    if (/*test_value == point_val && */ test_start == point_start){
      return seed;
    }
  }
  return 0;
}

uint64_t get_prediction(int difficulty){

  int tmp;
  uint64_t start;

  tmp = rand();
  start = (long)tmp << 0x20;

  tmp = rand();
  start = (start | (long)tmp) % (-difficulty - 2);

  tmp = rand();
  tmp = rand();
  uint64_t value = start + (ulong)(long)tmp % difficulty;

  tmp = rand();

  return value;
}


int prediction_holds(uint64_t prediction, unsigned char hash_to_find[]){
  unsigned char buffer[SHA256_DIGEST_LENGTH] = {0};
  unsigned char *found_hash = SHA256((const unsigned char *) &prediction, sizeof(prediction), buffer);
    if (memcmp(hash_to_find, found_hash, SHA256_DIGEST_LENGTH) == 0){
      return 1;
    }
  return 0;
}
