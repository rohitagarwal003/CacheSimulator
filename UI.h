#ifndef UI_H
#define	UI_H

#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>

using namespace std;

class UI {
public:

    UI();

    bool readTraceEntry(unsigned int& cmd, unsigned int& address, unsigned int& data, ifstream& fin);

    bool parseParams(int argc, char* argv[],
                     int& cacheCapacity,
                     int& cacheBlocksize,
                     int& cacheAssociativity,
                     char* replacement,
                     bool& verbose,
                     char* filename);

    virtual ~UI();
};

#endif	/* UI_H */
