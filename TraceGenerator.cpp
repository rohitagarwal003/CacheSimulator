#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>

using namespace std;

bool parseParams(int, char**, int&, int&, int&, int&, int&, int&);
double nextRandomExponential(double);
int generateAddresses(int, int, double, int, int, int, double, int);
int generateAddressesHelper(int, double, int, int, int, double, int);

const int MEM_SIZE = 1024 * 1024;

// Data accesses in a program are typically from a restricted memory domain.
int* dataAddressList;
int dataSize;

// There are only limited number of methods in a program.
int* startingAddressList;
int numMethods;

int main(int argc, char *argv[]) {

    int meanSequentialLength;
    int meanLoopLength;
    int meanLoopRepetitions;
    int percentDataInstructions;
    int percentWriteInstructions;
    int totalInstructions;

    bool parsed = parseParams(argc, argv,
                              totalInstructions,
                              meanSequentialLength,
                              meanLoopLength,
                              meanLoopRepetitions,
                              percentDataInstructions,
                              percentWriteInstructions);

    if (parsed) {

        // Seeding the random number generator
        srand((unsigned)time(NULL));

        // Assuming that the amount of data memory used by the program
        // depends linearly on the total number of instructions in the program.
        dataSize = totalInstructions/10;    
        dataAddressList = (int *) malloc(dataSize * sizeof(int));
        for (int i = 0; i < dataSize; i++) {
            dataAddressList[i] = MEM_SIZE/2 + rand() % (MEM_SIZE/2);
            // Assuming that data used by the program resides in the later half of the memory.
        }

        // Assuming that the number of methods in a program
        // depends linearly on the total number of instructions in the program.
        numMethods = totalInstructions/1000;
        startingAddressList = (int *) malloc(numMethods * sizeof(int));
        for (int i = 0; i < numMethods; i++) {
            startingAddressList[i] = rand() % (MEM_SIZE/2);
            // Assuming that instructions are stored in the earlier half of the memory.
        }

        int initialStartingAddress = 0;
        double initialJumpProbability = 0.1;

        generateAddresses(totalInstructions,
                          initialStartingAddress,
                          initialJumpProbability,
                          meanSequentialLength,
                          meanLoopLength,
                          meanLoopRepetitions,
                          percentDataInstructions/(double)100,
                          percentWriteInstructions);

        cout << endl;
    }
}

int generateAddresses(int totalInstructions, int startingAddress, double jumpProbability,
                      int meanSequentialLength, int meanLoopLength, int meanLoopRepetitions,
                      double dataInstructionProbability, int percentWriteInstructions) {

    int sequentialLength;
    int loopLength;
    int loopRepetitions;
    int instructionAddress = startingAddress;
    int numInstructions = 0;

    while (numInstructions < totalInstructions) {

        // The following assumes that all of sequential code length,
        // loop length and number of loop repetitions follow exponential distribution.
        sequentialLength = (int) nextRandomExponential(1.0/meanSequentialLength);
        loopLength       = (int) nextRandomExponential(1.0/meanLoopLength);
        loopRepetitions  = (int) nextRandomExponential(1.0/meanLoopRepetitions);

        // Sequential Part
        for (int i = 1; i <= sequentialLength; i++) {
            cout << "0 " << hex << instructionAddress + i << dec << endl;
            // Assuming that the function length gets smaller as we go deeper in the call stack.
            // Assuming that the probablity of calling another function decreases as we go deeper in the call stack.
            numInstructions += generateAddressesHelper((totalInstructions-numInstructions)/5,
                                                       jumpProbability/2,
                                                       meanSequentialLength,
                                                       meanLoopLength,
                                                       meanLoopRepetitions,
                                                       dataInstructionProbability,
                                                       percentWriteInstructions);
        }
        instructionAddress += sequentialLength;
        numInstructions    += sequentialLength;

        // Loop Part
        // TODO Bring some more regularity in the way addresses inside a loop are generated.
        // i.e. take care of the fact that if some instruction in a loop is a function call in one iteration,
        // then it is a function call in all iterations.
        for (int j = 0; j < loopRepetitions && loopLength > 0; j++) {
           for (int k = 1; k <= loopLength; k++) {
               cout << "0 " << hex << instructionAddress + k << dec << endl;
               // Assuming that the function length gets smaller as we go deeper in the call stack.
               // Assuming that the probablity of calling another function decreases as we go deeper in the call stack.
               numInstructions += generateAddressesHelper((totalInstructions-numInstructions)/5,
                                                          jumpProbability/2,
                                                          meanSequentialLength,
                                                          meanLoopLength,
                                                          meanLoopRepetitions,
                                                          dataInstructionProbability,
                                                          percentWriteInstructions);
           }
        }
        instructionAddress += loopRepetitions == 0? 0: loopLength;
        numInstructions    += (loopLength*loopRepetitions);
    }
    return numInstructions;
}

int generateAddressesHelper(int totalInstructions, double jumpProbability,
                            int meanSequentialLength, int meanLoopLength, int meanLoopRepetitions,
                            double dataInstructionProbability, int percentWriteInstructions) {

    double randomNum = (double)rand()/((double)RAND_MAX+1);
    int instructionsExecuted = 0;
    if (randomNum < jumpProbability) {
        // Simulating a method call in the program.
        int whichMethod = rand() % numMethods;
        int startingAddress = startingAddressList[whichMethod];
        instructionsExecuted = generateAddresses(totalInstructions,
                                                 startingAddress,
                                                 jumpProbability,
                                                 meanSequentialLength,
                                                 meanLoopLength,
                                                 meanLoopRepetitions,
                                                 dataInstructionProbability,
                                                 percentWriteInstructions);

    } else if (randomNum < jumpProbability + dataInstructionProbability) {
        if (rand() % 100 < percentWriteInstructions) {
            // Store/Write
            int location = rand() % dataSize;
            cout << "1 " << hex << dataAddressList[location] << dec << " " << rand() << endl;
        } else {
            // Load/Read
            int location = rand() % dataSize;
            cout << "0 " << hex << dataAddressList[location] << dec << endl;
        }
    }
    return instructionsExecuted;
}

double nextRandomExponential(double lambda) {

    // mean of the distribution is 1/lambda
    double rndU = ((double)rand()/((double)RAND_MAX+1));
    double expX = -1 * log(1-rndU) / lambda;
    return expX;
}

bool parseParams(int argc, char* argv[], int& totalInstructions,
                 int& meanSequentialLength, int& meanLoopLength, int& meanLoopRepetitions,
                 int& percentDataInstructions, int& percentWriteInstructions) {

    // For the parsing of command line options
    int c;
    bool errflg = false;
    // The following variables are used by getopt and are defined elsewhere so we just make them extern here.
    extern char *optarg;
    extern int optind, optopt, opterr;
    opterr = 0;

    while ((c = getopt(argc, argv, "t:s:l:r:d:w:")) != -1) {
        switch (c) {
            case 't':
                totalInstructions        = atoi(optarg);
                break;
            case 's':
                meanSequentialLength     = atoi(optarg);
                break;
            case 'l':
                meanLoopLength           = atoi(optarg);
                break;
            case 'r':
                meanLoopRepetitions      = atoi(optarg);
                break;
            case 'd':
                percentDataInstructions  = atoi(optarg);
                break;
            case 'w':
                percentWriteInstructions = atoi(optarg);
                break;
            case '?':
                if (optopt == 't' || optopt == 's' || optopt == 'l' || optopt == 'r' || optopt == 'd' || optopt == 'w') {
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
    char usageMessage[200];
    sprintf(usageMessage, "usage: %s -t <Total Instructions> -s <Mean Sequential Length> -l <Mean Loop Length> -r <Mean Loop Repetitions> -d <Percentage of Data Instructions> -w <Percentage of Write Instructions>\n", argv[0]);
    if (errflg) {
        fprintf(stderr, "%s", usageMessage);
        return false;
    } else if (optind != argc) {
        fprintf(stderr, "%s", usageMessage);
        return false;
    }
    return true;
}
