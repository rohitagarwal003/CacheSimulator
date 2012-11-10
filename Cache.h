/*
 * File:   Cache.h
 * Author: 3_Idiots
 *
 * Created on 4 November, 2010, 12:37 AM
 */

#ifndef CACHE_H
#define	CACHE_H

#include "Memory.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include <time.h>

class Cache {
public:

    Cache();

    Cache(int capacity, int blocksize, int associativity, char* replacement);

    Memory myMemory;

    int          capacity;
    int          blocksize;
    int          associativity;
    int          numberOfSets;
    int          blocksizeInWords;
    char         replacement[10];

    unsigned int **cache;
    unsigned int **tag;
    unsigned int **lastAccessed;
    bool         **valid;
    bool         **dirty;

    unsigned int reads;
    unsigned int writes;
    unsigned int readMiss;
    unsigned int writeMiss;
    unsigned int numDirtyBlocksEvicted;

    double memoryReadTime;
    double memoryWriteTime;
    double cacheAccessTime;
    double associativityFactor;
    double lruFactor;

    void setLRU(unsigned int index, unsigned int justAccessedBlock);

    unsigned int findLeastRecentlyUsedBlock(unsigned int index);

    void writeBack(unsigned int memory[],
                   unsigned int index,
                   unsigned int leastRecentlyUsedBlock);

    void readBlockFromMemory(unsigned int memory[],
                             unsigned int address,
                             unsigned int index,
                             unsigned int leastRecentlyUsedBlock);

    bool checkConfiguration();

    double write(int address, int data);

    double read(int address);

    void printCache();

    virtual ~Cache();
};

#endif	/* CACHE_H */
