//
// Created by Sandie Petersen on 16/11/2023.
//

#include "calculateHash.h"
#include "hashFinder.h"
#include <unistd.h>

void* calculate_hash(calculation_params_t * param) {

    while (param->start < param->end && param->keep_calculating) {
        //Pick 5 numbers and iterate for next time
        pthread_mutex_lock(param->lock);
        uint64_t start = param->start;
        uint64_t end = start + 0x1000;
        if (end > param->end) {
            end = param->end;
        }
        param->start = end;
        pthread_mutex_unlock(param->lock);

        find_hash(start, end, param);
    }

    pthread_exit(NULL); // Thread's job is done
}
