#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <openssl/sha.h>
#include "messages.h"



void set_enviroment(int env_value);
unsigned int predict_enviroment(uint64_t point_start ,int difficulty);
uint64_t get_prediction(int difficulty);
int prediction_holds(uint64_t prediction, unsigned char hash_to_find[]);
