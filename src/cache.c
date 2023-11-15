//
// Created by nts96 on 01/11/2023.
//

#include "cache.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

pthread_mutex_t cache_lock;
Element** map;


void insert(unsigned char* key, uint64_t value) {
    uint64_t index = calculate_index(key);
    pthread_mutex_lock(&cache_lock);
    Element *element = malloc(sizeof(Element));
    element->value = value;
    memcpy(&element->key, key, 32);

    if (map[index] == NULL ) {
        element->next = NULL;
        map[index] = element;
    } else {
        element->next = map[index];
        map[index] = element;
    }

    pthread_mutex_unlock(&cache_lock);
}


uint64_t search(unsigned char *searchKey) {

    uint64_t index = calculate_index(searchKey);
    uint64_t returnValue = 0;
    pthread_mutex_lock(&cache_lock);

    if (map[index] != NULL ) {
        Element *mapElement = map[index];
        while (mapElement != NULL) {
            int x = memcmp(mapElement->key, searchKey, 32);
            if (x == 0) {
                returnValue= mapElement->value;
                break;
            }
            mapElement = mapElement->next;
        }
    }

    pthread_mutex_unlock(&cache_lock);
    return returnValue;
}

uint64_t calculate_index(unsigned char* searchKey) {

    uint64_t value = 0;
    for (int i = 0; i < 8; ++i) {
        ((unsigned char*)&value)[i] = searchKey[i];
    }
    return value % CACHESIZE;
}

void initialize_cache() {
    map = (Element **) malloc(CACHESIZE * sizeof(Element));
    if(map == NULL) {
        printf("Problem initializing memory");
    }
    pthread_mutex_init(&cache_lock, NULL);
}

void free_cache() {
    Element* tmp;

    for (int i = 0; i < CACHESIZE; ++i) {
        if (map[i] == NULL) {
            continue;
        }

        while(map[i] != NULL) {
            tmp = map[i]->next;
            free(map[i]);
            map[i] = tmp;
        }
    }

    free(map);
    pthread_mutex_destroy(&cache_lock);
}