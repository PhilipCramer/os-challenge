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
    uint64_t index = calculateIndex(key);
    pthread_mutex_lock(&cache_lock);
    Element *element = malloc(sizeof(Element));
    element->value = value;
    memcpy(&element->key, key, 32);

    if (map[index] == NULL ) {
        element->next = NULL;
        map[index] = element;
        printf("Inserted value %"PRIu64 " at root\r\n", value);
    } else {
        element->next = map[index];
        map[index] = element;

        printf("Inserted value %"PRIu64 " nested\r\n", value);
    }

    pthread_mutex_unlock(&cache_lock);
}

uint64_t search(unsigned char *searchKey) {

    uint64_t index = calculateIndex(searchKey);
    uint64_t returnValue = 0;
    pthread_mutex_lock(&cache_lock);

    if (map[index] != NULL ) {
        Element *mapElement = map[index];
        printf("Found element at index %" PRIu64 " with the value %" PRIu64 "\r\n", index, mapElement->value);
        while (mapElement != NULL) {
            int x = memcmp(mapElement->key, searchKey, 32);
            printf("Comparing the values returned %d", x);
            if (x == 0) {
                returnValue= mapElement->value;
                printf("A key was found\r\n");
                break;
            }
            mapElement = mapElement->next;
        }
    }

    pthread_mutex_unlock(&cache_lock);
    return returnValue;
}

uint64_t calculateIndex(unsigned char* searchKey) {

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
    printf("Cache okay\r\n");
}