//
// Created by Alex Brodsky on 2024-03-10.
//

///The type of cache I will be implementing is directly mapped cache

/* This function is called from main()
 * It simulates a cache query for an 8 byte value.  It takes two parameters:
 *   address: The address of the long value being fetched.
 *   value: a pointer to where the value should be stored.
 * Returns: 1 on success and 0 if the address is not in range of value is NULL.
 */

#include "cache.h"
#include <math.h>

struct Line {
    int validBit;
    int tag;
    int data[32]; //as line cannot be more than 32 bytes long
};

struct set {
    struct Line *line;
};

struct cache {
    int initialize;
    struct set** cacheSets;
    //struct Line line[1];
    int numberSets;
};

static void init (struct cache *c, struct cache_info *cacheInfo) {
    if (c->initialize != 1) {
        int numSets = cacheInfo->F_size / (sizeof(struct set) + sizeof(struct Line));
// calculates number of sets on size of fast memory
        //int numSets = cacheInfo->F_size / 32;
        c->numberSets = numSets; //assign the number of sets
        c->cacheSets = (struct set**)cacheInfo->F_memory; //allocate memory for sets
        for (int i = 0; i < numSets; i++) {//allocate memory for lines in each set
            c->cacheSets[i] = cacheInfo->F_memory + sizeof(struct set);
            c->cacheSets[i]->line = cacheInfo->F_memory + ((sizeof(struct set))*c->numberSets) + (sizeof(struct Line) * i);
        }
        c->initialize = 1; //set the flag to 1
    }
}

int cache_get(unsigned long address, unsigned long* value) {
    struct cache c;

    if (c.initialize != 1) { //initialize if flag not set to 1
        init(&c,&c_info);
    }

    //calculating set index using the lsb (s bits)
    unsigned long setIndex = address % c.numberSets;

    //get line corresponding to the set index, this line will have our data
    struct Line *l = &(c.cacheSets[setIndex]->line);

    //unsigned long tag = address >> (c.numberSets + sizeof(struct Line));
    unsigned long temp = address >> 5;
    unsigned long index = temp >> c.numberSets;
    unsigned long tag = index;

    //unsigned long tag = 32 - pow(2, sizeof(l->data)) - c.numberSets;

    // Check if the tag matches and the line contains valid data(so not empty)
    if (l->tag == tag && l->validBit == 1) {
        unsigned long offset = address % sizeof(l->data); //extract the offset
        *value = *((unsigned long *) (l->data));//copy data from the cache line to the value pointer
        return 1; //cache hit
    } else {
         memget(address, l->data, sizeof(l->data));
            l->validBit = 1;
            l->tag = 0;
            // Extract the offset bits using the least significant bits (b bits)
            unsigned long offset = address % sizeof(l->data);
            *value = *((unsigned long *) (l->data));
            return 0; // miss
    }
}