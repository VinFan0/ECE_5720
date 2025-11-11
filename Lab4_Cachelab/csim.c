/*
 * Cache Lab
 * ECE 5720
 * Ryan Beck A02237765
 * Josh Christensen AXXXXXXXXX
 */
#include "cachelab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void printHelp();
void printError();
void printArgs();

int indexBits;
int lineCount;
int offsetBits;
char traceFile[16];

int main(int argc, char* argv[])
{
    // Flag check
    // Required flags: s, E, b, t
    // Optional flags: h
    if(argc > 1) {
       if(strcmp(argv[1], "-h") == 0) {
          printHelp();
          return 0;
       } else {
          if(argc < 9) {
	     printError();
	     printHelp();
	     return 0;
	  }
       }
    } else {
       printError();
       printHelp();
       return 0;
    }

    //Flag check succeeded
    indexBits = atoi(argv[2]);
    lineCount = atoi(argv[4]);
    offsetBits = atoi(argv[6]);
    strncpy(argv[6], traceFile, 16);
    traceFile[15] = '\0';

    printArgs();

    // Finish with this
    printSummary(0, 0, 0);
    return 0;
}

void printArgs() {
   printf("Set index bits:    %d\n", indexBits);
   printf("Lines per set:     %d\n", lineCount);
   printf("Block offset bits: %d\n", offsetBits);
   printf("Trace file name:   %s\n", traceFile);
}

void printHelp() {
// Print help message
   printf("Usage: ./csim -h -s <num> -E <num> -b <num> -t <file>\n");
   printf("Options:\n");
   printf("  -h\t     Print this help message.\n");
   printf("  -s <num>   Number of set index bits.\n");
   printf("  -E <num>   Number of lines per set.\n");
   printf("  -b <num>   Number of block offset bits.\n");
   printf("  -t <file>  Trace file.\n\n");
   printf("Example:\n");
   printf("  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
}

void printError() {
   printf("./csim: Missing required command line argument\n");
}
