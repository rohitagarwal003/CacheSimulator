/*
 * File:   main.cpp
 * Author: 3_Idiots
 *
 * Created on 4 November, 2010, 12:35 AM
 */
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>

#include "Cache.h"
#include "Memory.h"
#include "UI.h"

using namespace std;

enum {
    CACHE_READ, CACHE_WRITE
};

int main(int argc, char *argv[ ]) {

    unsigned int cmd     = 0;
    unsigned int address = 0;
    unsigned int data    = 0;

    int capacity      = 0;
    int blocksize     = 0;
    int associativity = 0;

    char replacement[10] = "Default";
    bool verbose         = false;

    ifstream fin;
    char filename[100];

    UI ui;
    if (!ui.parseParams(argc, argv, capacity, blocksize, associativity, replacement, verbose, filename)) {
        exit(2);
    }

    fin.open(filename);
    if (!fin) {
        fprintf(stderr, "%s cannot find file %s\n", argv[0], filename);
        exit(2);
    }

    Cache myCache(capacity, blocksize, associativity, replacement);
    if (!myCache.checkConfiguration()) {
        exit(2);
    }

    double readMissRate;
    double writeMissRate;
    double totalMissRate;

    cout << "Cache Capacity: "      << myCache.capacity      << endl;
    cout << "Cache BlockSize: "     << myCache.blocksize     << endl;
    cout << "Cache Associativity: " << myCache.associativity << endl;
    cout << "Number of Sets: "      << myCache.numberOfSets  << endl;
    cout << "Replacement Policy: "  << myCache.replacement   << endl;

    // Each iteration of this loop reads in and operates upon one transaction to memory.
    while (ui.readTraceEntry(cmd, address, data, fin)) {
        if (cmd == CACHE_WRITE) {
            // Process a cache write.
            myCache.write(address, data);
        } else {
            // Process a cache read.
            myCache.read(address);
        }
    }

    totalMissRate = (double) (myCache.readMiss + myCache.writeMiss) / (myCache.reads + myCache.writes);
    writeMissRate = (double) (myCache.writeMiss) / (myCache.writes);
    readMissRate  = (double) (myCache.readMiss) / (myCache.reads);

    if (verbose) {
        cout << "\nCACHE CONTENTS" << endl;
        myCache.printCache();
        cout << "\nMAIN MEMORY" << endl;
        myCache.myMemory.printMemory();
    }

    cout << "\nSTATISTICS" << endl;

    cout << "Total Misses:    "
         << setw(8) << myCache.readMiss + myCache.writeMiss
         << "\tRead Misses:    "
         << setw(8) << myCache.readMiss
         << "\tWrite Misses:    "
         << setw(8) << myCache.writeMiss
         << endl;

    cout << "Total Miss Rate: "
         << setw(8) << totalMissRate
         << "\tRead Miss Rate: "
         << setw(8) << readMissRate
         << "\tWrite Miss Rate: "
         << setw(8) << writeMissRate
         << endl;

    cout << "Number Of Dirty Blocks Evicted From The Cache: " << myCache.numDirtyBlocksEvicted << endl;
}
