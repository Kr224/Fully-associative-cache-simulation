//
// Created by Alex Brodsky on 2024-03-10.
//

///The type of cache I will be implementing is directly mapped cache

#include "cache.h"
#include "math.h"

struct Line {
    int validBit;
    unsigned long tag;
    void *b;
};

struct cache {
    int initialize;
    struct Line* line;
    int numberSets;
};

static void init(struct cache *c, struct cache_info *cacheInfo) {
    if (c->initialize != 1) {
        c->initialize = 1; // Set the flag to 1
        unsigned int numSets = cacheInfo->F_size / 32; // Calculates number of sets based on fast memory size
        c->numberSets = numSets; // Assign the number of sets
        c->line = (struct Line*)(cacheInfo->F_memory + sizeof(struct cache)); // Allocating memory for each line separately
        for (int i = 0; i < numSets; i++) {
            c->line[i].b = (void*)((char*)(cacheInfo->F_memory) + sizeof(struct cache) + sizeof(struct Line) * numSets + (32 * i));
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

    unsigned long offset = address >> 5; //for 32 bit adresses
    unsigned long index = log2(c_info.F_size / 32);
    unsigned long tag = log2(c_info.M_size) - offset - index;

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