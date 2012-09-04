/*
 * File:   UI.cpp
 * Author: 3_Idiots
 * Special Thanks to Prof. Doug Burger (creator of simplescalar)
 * 
 * Created on 4 November, 2010, 2:17 AM
 */

#include "UI.h"

using namespace std;

enum {
    CACHE_READ, CACHE_WRITE
};

UI::UI() {
}

bool UI::readTraceEntry(unsigned int& cmd, unsigned int& address, unsigned int& data, ifstream& fin) {
    // Parse one line of the trace file and insert the data read in into our arguments. Return true if a full record was read.
    fin >> cmd >> hex >> address >> dec;
    if (cmd == CACHE_WRITE) {
        // We have an extra field to read.
        fin >> hex >> data >> dec;
    }
    return !fin.eof();
}

bool UI::parseParams(int argc, char* argv[], int& cacheCapacity, int& cacheBlocksize, int& cacheAssociativity, char* replacement, bool& verbose, char* filename) {
    // For the parsing of command line options
    int c;
    bool errflg = false;
    // The following variables are used by getopt and are defined elsewhere so we just make them extern here.
    extern char *optarg;
    extern int optind, optopt, opterr;
    opterr = 0;

    while ((c = getopt(argc, argv, "c:b:a:r:v")) != -1) {
        switch (c) {
            case 'c':
                cacheCapacity = atoi(optarg);
                break;
            case 'b':
                cacheBlocksize = atoi(optarg);
                break;
            case 'a':
                cacheAssociativity = atoi(optarg);
                break;
            case 'r':
                strcpy(replacement, optarg);
                break;
            case 'v':
                verbose = true;
                break;
            case '?':
                if (optopt == 'c' || optopt == 'b' || optopt == 'a' || optopt == 'r') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                    errflg = true;
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                    errflg = true;
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                    errflg = true;
                }
                break;
        }
    }
    // Check that we have no illegal options.
    if (errflg) {
        fprintf(stderr, "usage: %s -c<capacity> -b<blocksize> -a<associativity> -r<replacement policy> -v<verbose> <input_trace>\n", argv[0]);
        return false;
    } else {
        if (optind != argc - 1) {
            fprintf(stderr, "usage: %s -c<capacity> -b<blocksize> -a<associativity> -r<replacement policy> -v<verbose> <input_trace>\n", argv[0]);
            return false;
        }
        strcpy(filename, argv[optind]);
        return true;
    }
}

UI::~UI() {
}