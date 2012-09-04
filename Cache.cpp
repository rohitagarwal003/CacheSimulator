/*
 * File:   Cache.cpp
 * Author: 3_Idiots
 *
 * Created on 4 November, 2010, 12:37 AM
 */

#include "Cache.h"

using namespace std;

Cache::Cache() {
}

Cache::Cache(int cap, int block, int asso, char* repl) {

    capacity = (cap == 0) ? 8 : cap;
    blocksize = (block == 0) ? 16 : block;
    associativity = (asso == 0) ? 4 : asso;
    if (!strcmp(repl, "Default")) {
        strcpy(replacement, "LRU");
    } else {
        strcpy(replacement, repl);
        srand(time(NULL));
    }
    numberOfSets = 1024 * capacity / (associativity * blocksize);
    blocksizeInWords = blocksize / 4;
    reads = 0, writes = 0, readMiss = 0, writeMiss = 0, numDirtyBlocksEvicted = 0;

    cache = new unsigned int* [numberOfSets];
    for (int i = 0; i < numberOfSets; i++) {
        cache[i] = new unsigned int[associativity * blocksize];
    }
    tag = new unsigned int* [numberOfSets];
    for (int i = 0; i < numberOfSets; i++) {
        tag[i] = new unsigned int[associativity];
    }
    lastAccessed = new unsigned int* [numberOfSets];
    for (int i = 0; i < numberOfSets; i++) {
        lastAccessed[i] = new unsigned int[associativity];
    }
    valid = new bool* [numberOfSets];
    for (int i = 0; i < numberOfSets; i++) {
        valid[i] = new bool[associativity];
    }
    dirty = new bool* [numberOfSets];
    for (int i = 0; i < numberOfSets; i++) {
        dirty[i] = new bool[associativity];
    }

    for (int i = 0; i < numberOfSets; i++) {
        for (int k = 0; k < associativity; k++) {
            valid[i][k] = false;
            dirty[i][k] = false;
            lastAccessed[i][k] = 0;
            tag[i][k] = 0;
        }
    }
}

bool Cache::checkConfiguration() {
    if (!(capacity == 4 || capacity == 8 || capacity == 16 || capacity == 32 || capacity == 64)) {
        fprintf(stderr, "Cache Capacity should be either 4, 8, 16, 32 or 64 kbytes\n");
        return false;
    }
    if (!(blocksize == 4 || blocksize == 8 || blocksize == 16 || blocksize == 32 || blocksize == 64 || blocksize == 128 || blocksize == 256 || blocksize == 512)) {
        fprintf(stderr, "Cache Block Size should be either 4, 8, 16, 32, 64, 128, 256 or 512 bytes\n");
        return false;
    }
    if (!(associativity == 1 || associativity == 2 || associativity == 4 || associativity == 8 || associativity == 16)) {
        fprintf(stderr, "Cache Associativity should be either 1, 2, 4, 8 or 16\n");
        return false;
    }
    if (!(!strcmp(replacement, "LRU") || !strcmp(replacement, "random"))) {
        fprintf(stderr, "Replacement Policy should be either LRU or random\n");
        return false;
    }
    return true;
}

void Cache::setLRU(unsigned int index, unsigned int justAccessedBlock) {
    for (int k = 0; k < associativity; k++) {
        if (valid[index][k] && lastAccessed[index][k] > lastAccessed[index][justAccessedBlock]) {
            lastAccessed[index][k]--;
        }
    }
    lastAccessed[index][justAccessedBlock] = associativity;
}

unsigned int Cache::findLeastRecentlyUsedBlock(unsigned int index) {
    unsigned int leastRecentlyUsedBlock = 0;
    for (int k = 0; k < associativity; k++) {
        if (lastAccessed[index][k] < lastAccessed[index][leastRecentlyUsedBlock]) {
            leastRecentlyUsedBlock = k;
        }
    }
    return leastRecentlyUsedBlock;
}

void Cache::writeBack(unsigned int memory[], unsigned int index, unsigned int someBlock) {
    unsigned int presentTag = tag[index][someBlock];
    unsigned int writeBackAddress = presentTag * (numberOfSets * blocksizeInWords) + (index * blocksizeInWords);
    for (int l = 0; l < blocksizeInWords; l++) {
        memory[writeBackAddress + l] = cache[index][someBlock * blocksizeInWords + l];
    }
}

void Cache::readBlockFromMemory(unsigned int memory[], unsigned int address, unsigned int index, unsigned int someBlock) {
    unsigned int presentTag = address / (blocksizeInWords * numberOfSets);
    unsigned int readAddress = presentTag * (numberOfSets * blocksizeInWords) + (index * blocksizeInWords);
    for (int l = 0; l < blocksizeInWords; l++) {
        cache[index][someBlock * blocksizeInWords + l] = memory[readAddress + l];
    }
    tag[index][someBlock] = presentTag;
    valid[index][someBlock] = true;
}

void Cache::printCache() {
    cout << "Set Number " << "Valid " << "  Tag " << "Dirty ";
    for (int k = 0; k < blocksizeInWords; k++) {
        cout << setw(8) << "word" << setw(2) << k;
    }
    cout << endl;
    for (int i = 0; i < numberOfSets; i++) {
        for (int j = 0; j < associativity; j++) {
            cout << dec << setw(10) << i;
            cout << setw(6) << valid[i][j] ? "T" : "F";
            cout << hex << setw(6) << tag[i][j];
            cout << setw(6) << dirty[i][j] ? "T" : "F";
            for (int k = 0; k < blocksizeInWords; k++) {
                cout << hex << setw(10) << cache[i][j * blocksizeInWords + k];
            }
            cout << endl;
        }
        cout << endl;
    }
    cout << dec;
}

void Cache::write(int address, int data) {
    bool isHit = false;
    unsigned int index = (address / blocksizeInWords) % numberOfSets;
    unsigned int wordPositionInBlock = address % blocksizeInWords;
    writes++;
    unsigned int presentTag = address / (blocksizeInWords * numberOfSets);
    for (int j = 0; j < associativity; j++) {
        if ((tag[index][j] == presentTag) && valid[index][j]) {//It is a hit.
            cache[index][j * blocksizeInWords + wordPositionInBlock] = data;
            dirty[index][j] = true;
            isHit = true;
            if (!strcmp(replacement, "LRU")) {
                setLRU(index, j);
            }
            break;
        }
    }
    if (!isHit) {
        writeMiss++;
        if (!strcmp(replacement, "LRU")) {
            unsigned int leastRecentlyUsedBlock = findLeastRecentlyUsedBlock(index);
            if (dirty[index][leastRecentlyUsedBlock] && valid[index][leastRecentlyUsedBlock]) {
                numDirtyBlocksEvicted++;
                writeBack(myMemory.memory, index, leastRecentlyUsedBlock);
            }
            readBlockFromMemory(myMemory.memory, address, index, leastRecentlyUsedBlock);
            //Write word to cache.
            cache[index][leastRecentlyUsedBlock * blocksizeInWords + wordPositionInBlock] = data;
            dirty[index][leastRecentlyUsedBlock] = true;
            setLRU(index, leastRecentlyUsedBlock);
        } else {
            unsigned int someBlock = rand() % associativity;
            if (dirty[index][someBlock] && valid[index][someBlock]) {
                numDirtyBlocksEvicted++;
                writeBack(myMemory.memory, index, someBlock);
            }
            readBlockFromMemory(myMemory.memory, address, index, someBlock);
            //Write word to cache.
            cache[index][someBlock * blocksizeInWords + wordPositionInBlock] = data;
            dirty[index][someBlock] = true;
        }
    }
}

void Cache::read(int address) {
    bool isHit = false;
    unsigned int index = (address / blocksizeInWords) % numberOfSets;
    reads++;
    unsigned int presentTag = address / (blocksizeInWords * numberOfSets);
    for (int j = 0; j < associativity; j++) {
        if ((tag[index][j] == presentTag) && valid[index][j]) {//It is a hit.
            isHit = true;
            if (!strcmp(replacement, "LRU")) {
                setLRU(index, j);
            }
            break;
        }
    }
    if (!isHit) {
        readMiss++;
        if (!strcmp(replacement, "LRU")) {
            unsigned int leastRecentlyUsedBlock = findLeastRecentlyUsedBlock(index);
            if (dirty[index][leastRecentlyUsedBlock] && valid[index][leastRecentlyUsedBlock]) {
                numDirtyBlocksEvicted++;
                writeBack(myMemory.memory, index, leastRecentlyUsedBlock);
            }
            readBlockFromMemory(myMemory.memory, address, index, leastRecentlyUsedBlock);
            dirty[index][leastRecentlyUsedBlock] = 0;
            setLRU(index, leastRecentlyUsedBlock);
        } else {
            unsigned int someBlock = rand() % associativity;
            if (dirty[index][someBlock] && valid[index][someBlock]) {
                numDirtyBlocksEvicted++;
                writeBack(myMemory.memory, index, someBlock);
            }
            readBlockFromMemory(myMemory.memory, address, index, someBlock);
            dirty[index][someBlock] = 0;
        }
    }
}

Cache::~Cache() {
}