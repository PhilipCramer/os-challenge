#include "hashFinder.h"
#include <string.h>
#include "stdbool.h"
#include "calculateHash.h"

void find_hash(uint64_t start, uint64_t stop, calculation_params_t * param) {
    unsigned char *found_hash;
    uint64_t current;
    unsigned char buffer[32];
    for (current = start; current < stop; current++) {

        found_hash = SHA256((const unsigned char *) &current, sizeof(current), buffer);


        //If solution is found
        if (memcmp(param->hash, buffer, SHA256_DIGEST_LENGTH) == 0) {
            pthread_mutex_lock(param->lock);
            *param->response = current;
            //Stop calculating and send signal to consumer thread
            param->keep_calculating = false;
            pthread_cond_signal(param->done);
            pthread_mutex_unlock(param->lock);
            return;
        }
    }

    return;
}
