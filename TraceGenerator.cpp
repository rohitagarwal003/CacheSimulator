#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>

using namespace std;
bool parseParams(int, char**, int&, int&, int&, int&, int&);
double nextRandomExponential(double);

int main(int argc, char *argv[]) {
    int meanSequentialLength;
    int meanLoopLength;
    int meanLoopRepetitions;
    int percentDataInstructions;
    int percentWriteInstructions;
    int sequentialLength;
    int loopLength;
    int loopRepetitions;
    if (parseParams(argc, argv, meanSequentialLength, meanLoopLength, meanLoopRepetitions, percentDataInstructions, percentWriteInstructions)) {
        srand((unsigned)time(NULL));
        int instructionAddress = 0;
        int numInstructions    = 0;
        while (numInstructions < 1000) {
            sequentialLength = (int)nextRandomExponential(1/(double)meanSequentialLength);
            loopLength       = (int)nextRandomExponential(1/(double)meanLoopLength);
            loopRepetitions  = (int)nextRandomExponential(1/(double)meanLoopRepetitions);
            for (int i = 1; i <= sequentialLength; i++) {
                cout << instructionAddress + i << " ";
            }
            instructionAddress += sequentialLength;
            numInstructions    += sequentialLength;
            cout << endl;
            for (int j = 0; j < loopRepetitions; j++) {
               for (int k = 1; k <= loopLength; k++) {
                   cout << instructionAddress + k << " ";
               }
               cout << endl;
            }
            cout << endl;
            instructionAddress += loopRepetitions == 0? 0: loopLength;
            numInstructions    += (loopLength*loopRepetitions);
        }
        cout << endl;
    }
}

double nextRandomExponential(double lambda) {
    // mean of the distribution is 1/lambda
    double rndU = ((double)rand()/((double)RAND_MAX+1));
    double expX = -1 * log(1-rndU) / lambda;
    return expX;
}

bool parseParams(int argc, char* argv[], int& meanSequentialLength, int& meanLoopLength, int& meanLoopRepetitions, int& percentDataInstructions, int& percentWriteInstructions) {
    // For the parsing of command line options
    int c;
    bool errflg = false;
    // The following variables are used by getopt and are defined elsewhere so we just make them extern here.
    extern char *optarg;
    extern int optind, optopt, opterr;
    opterr = 0;

    while ((c = getopt(argc, argv, "s:l:r:d:w:")) != -1) {
        switch (c) {
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
                if (optopt == 's' || optopt == 'l' || optopt == 'r' || optopt == 'd' || optopt == 'w') {
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
    sprintf(usageMessage, "usage: %s -s<Mean Sequential Length> -l<Mean Loop Length> -r<Mean Loop Repetitions> -d<Percentage of Data Instructions> -w<Percentage of Write Instructions>\n", argv[0]);
    if (errflg) {
        fprintf(stderr, "%s", usageMessage);
        return false;
    } else if (optind != argc) {
        fprintf(stderr, "%s", usageMessage);
        return false;
    }
    return true;
}
