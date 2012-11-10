/*
 * File:   Memory.cpp
 * Author: 3_Idiots
 *
 * Created on 4 November, 2010, 12:40 AM
 */

#include "Memory.h"

using namespace std;

Memory::Memory() {
    for (int i = 0; i < MAX_MEM_SIZE; i++) {
        memory[i] = i;
    }
}

void Memory::printMemory() {
    for (int m = 0; m < MAX_MEM_SIZE; m = m + 8) {
        cout << hex
             << setw(12) << m
             << dec
             << setw(12) << memory[m]
             << setw(12) << memory[m + 1]
             << setw(12) << memory[m + 2]
             << setw(12) << memory[m + 3]
             << setw(12) << memory[m + 4]
             << setw(12) << memory[m + 5]
             << setw(12) << memory[m + 6]
             << setw(12) << memory[m + 7]
             << endl;
    }
}

Memory::~Memory() {
}
