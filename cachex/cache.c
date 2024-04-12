//
// Created by Alex Brodsky on 2024-03-10.
//

#include "cache.h"
#include "math.h"

struct Line {
    int validBit;
    unsigned long tag;
    void *b;
};

struct cache {
    int initialize;
    int numberSets;
    struct Line* line;
};

static void init(struct cache *c, struct cache_info *cacheInfo) {
    if (c->initialize != 1) {
        c->initialize = 1; // Set the flag to 1
        unsigned int numSets = (cacheInfo->F_size - sizeof(struct cache)) / sizeof(struct Line); // Calculates number of sets based on fast memory size
        c->numberSets = numSets; // Assign the number of sets
        c->line = (struct Line*)(cacheInfo->F_memory + sizeof(struct cache)); // Allocating memory for each line separately
        for (int i = 0; i < numSets; i++) {
            c->line[i].b = (void*)((char*)(cacheInfo->F_memory) + sizeof(struct cache)
                    + sizeof(struct Line) * numSets + (32 * i));
        }
    }
}

int cache_get(unsigned long address, unsigned long* value) {
    struct cache *c = (struct cache*)c_info.F_memory; //casting to cache pointer
    init(c, &c_info);

    // Calculating set index
    unsigned long setIndex = address % c->numberSets;

    //getting line corresponding to the set index
    struct Line *l = &(c->line[setIndex]);

    int numLines = c_info.F_size / c_info.M_size; // to get my cache lines

    unsigned int offsetBits = 0;
    while ((1 << offsetBits) < numLines) { //no. of bits i need for my offset iside the cache line
        offsetBits++;
    }
    unsigned int indexBits = 0;
    while ((1 << indexBits) < numLines) {
        indexBits++;
    }

    unsigned int offsetMask = (1 << offsetBits) - 1;
    unsigned long tag = address >> (offsetBits + indexBits);
    unsigned int offset = address & offsetMask;


    // Check if the tag matches and the line contains valid data (not empty)
    if (l->tag == tag && l->validBit == 1) {
        *value = *((unsigned long *)(l->b) + offset);
        return 1; // Cache hit
    } else {
        memget(address, l->b, sizeof(l->b));
        l->validBit = 1;
        l->tag = tag;
        *value = *((unsigned long *)(l->b) + offset);
        return 0; // Miss
    }
}









