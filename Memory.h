#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <iomanip>

const int MAX_MEM_SIZE = 1024 * 1024;

class Memory {
  public:
    Memory();
    unsigned int memory[MAX_MEM_SIZE];
    void printMemory();
    virtual ~Memory();
};

#endif /* MEMORY_H */
