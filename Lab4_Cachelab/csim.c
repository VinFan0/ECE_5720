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

// DEBUG AND HELPER FUNCTIONS
void printHelp();
void printError();
void printArgs();

// CACHE SIMULATION FUNCTIONS
cache* createCache(int s, int E, int b);
void freeCache(cache* c);

// CACHE SIMULATION VARIABLES AND TYPES
int indexBits;
int lineCount;
int offsetBits;
char traceFile[32];

typedef struct {
    bool valid;
    int tag;
    int block;
} line;

typedef struct {
    line* lines;
} set;

typedf struct {
    set* sets;
    int s;
    int E;
    int b;
} cache;


// MAIN FUNCTION CODE
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
    // Collect input argments in global variables
    indexBits = atoi(argv[2]);
    lineCount = atoi(argv[4]);
    offsetBits = atoi(argv[6]);
   
    strncpy(traceFile, argv[8], sizeof(traceFile) - 1);
    traceFile[sizeof(traceFile)-1] = '\0';
    
    // DEBUG: display input arguments
    printArgs();

    // Generate Cache Table
    cache myCache = creatCache(indexBits, lineCount, offsetBits);

    // Iterate through traceFile lines

    // Deallocate cache
    freeCache(myCache);

    // Get summary for grading
    printSummary(0, 0, 0);
    return 0;
}

/*
 * Function: 	createCache
 * Input:	int <s> - number of set index bits
 * 		int <E> - number of lines per set (associativity)
 * 		int <b> - number of block offset bits
 * Output:	cache*	- pointer to cache structure
 * Description:
 * Take input arguments for cache parameters <s>, <E>, <b> and
 * dynamically allocate the simulation cache, with its respective
 * sets and lines. Initialize each line as
 * 	valid	= false
 * 	tag	= -1
 * 	block	= 0 
 */
cache* createCache(int s, int E, int b) {
    cache* c = malloc(sizeof(cache));
    c->s = s;
    c->E = E;
    c->b = b;

    int S = 1 << s;
    c->sets = malloc(S * sizeof(set));

    for (int i=0; i < S; i++) {
    	c->sets[i].lines = malloc(E * sizeof(line));
	for (int j = 0; j < E; j++) {
	    c->sets[i].lines[j].valid = false;
	    c->sets[i].lines[j].tag = -1;
	    c->sets[i].lines[j].block = 0;
	}
    }

    return c;
}


/*
 * Function:	freeCache
 * Input:	cahe* <c> - pointer to cache structure
 * Output:	void
 * Description:
 * Take input argment <c> and deallocate memory used by the cache.
 * Deallocation occurs in the following order
 * 	> lines
 * 	> sets
 * 	> cache
 */
void freeCache(cache* c) {
    int S = 1 << c->s;
    for (int i = 0; i < S; i++) {
    	free(c->sets[i].lines);
    }
    free(c->sets);
    free(c);
}

/*
 * Function:	printArgs
 * Input:	void
 * output:	void
 * Description:
 * (DEBUG) Print the values of the following global variables to the console.
 * 	> indexBits
 * 	> lineCount
 * 	> offsetBits
 * 	> traceFile
 */
void printArgs() {
   printf("Set index bits:    %d\n", indexBits);
   printf("Lines per set:     %d\n", lineCount);
   printf("Block offset bits: %d\n", offsetBits);
   printf("Trace file name:   %s\n", traceFile);
}

/*
 * Function:	printHelp
 * Input:	void
 * Output:	void
 * Description:
 * (HELPER) print the usage message for the csim program. Used when input arguments
 * are malformed, or the -h flag is included.
 */
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

/*
 * Function:	printError
 * Input:	void
 * Output:	void
 * Description:
 * (HELPER) print the error message for malformed input arguments.
 */
void printError() {
   printf("./csim: Missing required command line argument\n");
}
