#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>

using namespace std;
bool parseParams(int, char**, int&, int&, int&, int&, int&, int&);
double nextRandomExponential(double);
int generateAddresses(int, int, double, int, int, int);

int main(int argc, char *argv[]) {
    int meanSequentialLength;
    int meanLoopLength;
    int meanLoopRepetitions;
    int percentDataInstructions;
    int percentWriteInstructions;
    int totalInstructions;
    if (parseParams(argc, argv, totalInstructions, meanSequentialLength, meanLoopLength, meanLoopRepetitions, percentDataInstructions, percentWriteInstructions)) {
        srand((unsigned)time(NULL));
        int instructionAddress = 0;
        generateAddresses(totalInstructions,
                          instructionAddress,
                          0.1,
                          meanSequentialLength,
                          meanLoopLength,
                          meanLoopRepetitions);
        cout << endl;
    }
}

int generateAddresses(int totalInstructions, int startingAddress, double jumpProbability, int meanSequentialLength, int meanLoopLength, int meanLoopRepetitions) {
    int sequentialLength;
    int loopLength;
    int loopRepetitions;
    int instructionAddress = startingAddress;
    int numInstructions = 0;
    while (numInstructions < totalInstructions) {
        // The following assumes that all of sequential code length,
        // loop length and number of loop repetitions follow exponential distribution.
        sequentialLength = (int)nextRandomExponential(1/(double)meanSequentialLength);
        loopLength       = (int)nextRandomExponential(1/(double)meanLoopLength);
        loopRepetitions  = (int)nextRandomExponential(1/(double)meanLoopRepetitions);
        for (int i = 1; i <= sequentialLength; i++) {
            cout << "0 " << hex << instructionAddress + i << dec << endl;
            if ((double)rand()/((double)RAND_MAX+1) < jumpProbability) {
                int instructionsExecuted = generateAddresses((totalInstructions-numInstructions)/5,
                                                             rand() % (1024*1024),
                                                             jumpProbability/2,
                                                             meanSequentialLength,
                                                             meanLoopLength,
                                                             meanLoopRepetitions);
                numInstructions += instructionsExecuted;

            }
        }
        instructionAddress += sequentialLength;
        numInstructions    += sequentialLength;
        for (int j = 0; j < loopRepetitions && loopLength > 0; j++) {
           for (int k = 1; k <= loopLength; k++) {
               cout << "0 " << hex << instructionAddress + k << dec << endl;
           }
        }
        instructionAddress += loopRepetitions == 0? 0: loopLength;
        numInstructions    += (loopLength*loopRepetitions);
    }
    return numInstructions;
}

double nextRandomExponential(double lambda) {
    // mean of the distribution is 1/lambda
    double rndU = ((double)rand()/((double)RAND_MAX+1));
    double expX = -1 * log(1-rndU) / lambda;
    return expX;
}

bool parseParams(int argc, char* argv[], int& totalInstructions, int& meanSequentialLength, int& meanLoopLength, int& meanLoopRepetitions, int& percentDataInstructions, int& percentWriteInstructions) {
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
    sprintf(usageMessage, "usage: %s -t<Total Instructions> -s<Mean Sequential Length> -l<Mean Loop Length> -r<Mean Loop Repetitions> -d<Percentage of Data Instructions> -w<Percentage of Write Instructions>\n", argv[0]);
    if (errflg) {
        fprintf(stderr, "%s", usageMessage);
        return false;
    } else if (optind != argc) {
        fprintf(stderr, "%s", usageMessage);
        return false;
    }
    return true;
}
