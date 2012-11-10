/*
 * File:   UI.h
 * Author: 3_Idiots
 *
 * Created on 4 November, 2010, 2:17 AM
 */

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
