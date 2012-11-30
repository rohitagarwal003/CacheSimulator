#include "./Cache.h"

using namespace std;

Cache::Cache() {
}

Cache::Cache(int cap, int block, int asso, char* repl) {

    capacity      = (cap   == 0) ?  8 : cap;
    blocksize     = (block == 0) ? 16 : block;
    associativity = (asso  == 0) ?  4 : asso;

    if (!strcmp(repl, "Default")) {
        strcpy(replacement, "LRU");
    } else {
        strcpy(replacement, repl);
        srand(time(NULL));
    }

    numberOfSets     = 1024 * capacity / (associativity * blocksize);
    blocksizeInWords = blocksize / 4;

    reads = 0;
    writes = 0;
    readMiss = 0;
    writeMiss = 0;
    numDirtyBlocksEvicted = 0;
    validCount = 0;

    memoryReadTime      = 50;
    memoryWriteTime     = 50;
    cacheAccessTime     =  5;
    lruFactor           = 0.02;
    if (associativity != 1) {
        associativityFactor = 1 + 0.05*associativity;
    } else {
        associativityFactor = 0;
    }

    cache = new unsigned int* [numberOfSets];
    for (int i = 0; i < numberOfSets; i++) {
        cache[i] = new unsigned int[associativity * blocksize];
    }

    tag          = new unsigned int* [numberOfSets];
    lastAccessed = new unsigned int* [numberOfSets];
    valid        = new bool* [numberOfSets];
    dirty        = new bool* [numberOfSets];

    for (int i = 0; i < numberOfSets; i++) {
        tag[i]          = new unsigned int[associativity];
        lastAccessed[i] = new unsigned int[associativity];
        valid[i]        = new bool[associativity];
        dirty[i]        = new bool[associativity];
    }

    for (int i = 0; i < numberOfSets; i++) {
        for (int k = 0; k < associativity; k++) {
            valid[i][k]        = false;
            dirty[i][k]        = false;
            lastAccessed[i][k] = 0;
            tag[i][k]          = 0;
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
    if (valid[index][someBlock] == false) {
         validCount++;
    }
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
                cout << dec << setw(12) << cache[i][j * blocksizeInWords + k];
            }
            cout << endl;
        }
        cout << endl;
    }
    cout << dec;
}

double Cache::write(int address, int data) {

    writes++;

    bool isHit = false;
    unsigned int index = (address / blocksizeInWords) % numberOfSets;
    unsigned int wordPositionInBlock = address % blocksizeInWords;
    unsigned int presentTag = address / (blocksizeInWords * numberOfSets);

    double time = (cacheAccessTime * associativityFactor);

    for (int j = 0; j < associativity; j++) {
        if ((tag[index][j] == presentTag) && valid[index][j]) {
            // It is a hit.
            cache[index][j * blocksizeInWords + wordPositionInBlock] = data;
            dirty[index][j] = true;
            isHit = true;
            if (!strcmp(replacement, "LRU")) {
                setLRU(index, j);
                time += (cacheAccessTime * associativityFactor * lruFactor);  // Hit time depends on replacement policy.
            }
            break;
        }
    }

    if (!isHit) {
        writeMiss++;

        if (!strcmp(replacement, "LRU")) {
            unsigned int leastRecentlyUsedBlock = findLeastRecentlyUsedBlock(index);
            time += (cacheAccessTime * associativityFactor * lruFactor);  // Miss time depends on replacement policy.

            if (dirty[index][leastRecentlyUsedBlock] && valid[index][leastRecentlyUsedBlock]) {
                numDirtyBlocksEvicted++;
                time += memoryWriteTime;     // If block is dirty we need to write to memory.
                writeBack(myMemory.memory, index, leastRecentlyUsedBlock);
            }
            time += memoryReadTime;
            readBlockFromMemory(myMemory.memory, address, index, leastRecentlyUsedBlock);

            // Write word to cache.
            cache[index][leastRecentlyUsedBlock * blocksizeInWords + wordPositionInBlock] = data;
            dirty[index][leastRecentlyUsedBlock] = true;
            setLRU(index, leastRecentlyUsedBlock);

        } else {
            unsigned int someBlock = rand() % associativity;

            if (dirty[index][someBlock] && valid[index][someBlock]) {
                numDirtyBlocksEvicted++;
                time += memoryWriteTime;     // If block is dirty we need to write to memory.
                writeBack(myMemory.memory, index, someBlock);
            }
            time += memoryReadTime;
            readBlockFromMemory(myMemory.memory, address, index, someBlock);

            // Write word to cache.
            cache[index][someBlock * blocksizeInWords + wordPositionInBlock] = data;
            dirty[index][someBlock] = true;
        }
    }
    return time;
}

double Cache::read(int address) {

    reads++;
    bool isHit = false;
    double time = 0;
    unsigned int index = (address / blocksizeInWords) % numberOfSets;
    unsigned int presentTag = address / (blocksizeInWords * numberOfSets);

    time += (cacheAccessTime * associativityFactor);     // Hit time depends on associativity.

    for (int j = 0; j < associativity; j++) {
        if ((tag[index][j] == presentTag) && valid[index][j]) {
            // It is a hit.
            isHit = true;
            if (!strcmp(replacement, "LRU")) {
                setLRU(index, j);
                time += (cacheAccessTime * associativityFactor * lruFactor);  // Hit time depends on replacement policy.
            }
            break;
        }
    }

    if (!isHit) {

        readMiss++;

        if (!strcmp(replacement, "LRU")) {

            time += (cacheAccessTime * associativityFactor * lruFactor);  // Miss time depends on replacement policy.

            unsigned int leastRecentlyUsedBlock = findLeastRecentlyUsedBlock(index);

            if (dirty[index][leastRecentlyUsedBlock] && valid[index][leastRecentlyUsedBlock]) {
                time += memoryWriteTime;     // If block is dirty we need to write to memory.
                numDirtyBlocksEvicted++;
                writeBack(myMemory.memory, index, leastRecentlyUsedBlock);
            }

            time += memoryReadTime;    // Miss penalty - we have to read from memory..
            readBlockFromMemory(myMemory.memory, address, index, leastRecentlyUsedBlock);
            dirty[index][leastRecentlyUsedBlock] = 0;
            setLRU(index, leastRecentlyUsedBlock);

        } else {
            unsigned int someBlock = rand() % associativity;

            if (dirty[index][someBlock] && valid[index][someBlock]) {
                time += memoryWriteTime;     // If block is dirty we need to write to memory.
                numDirtyBlocksEvicted++;
                writeBack(myMemory.memory, index, someBlock);
            }

            time += memoryReadTime;    // Miss penalty - we have to read from memory..
            readBlockFromMemory(myMemory.memory, address, index, someBlock);
            dirty[index][someBlock] = 0;
        }
    }

    return time;
}

Cache::~Cache() {
}
