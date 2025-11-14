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
#include <stdbool.h>
#include <stdint.h>

// CACHE SIMULATION VARIABLES AND TYPES
int indexBits;
int lineCount;
int offsetBits;
char traceFile[32];

typedef struct {
    bool valid;
    int tag;
    int block;
    int accessTime;
} line;

typedef struct {
    line* lines;
} set;

typedef struct {
    set* sets;
    int s;
    int E;
    int b;
    unsigned long long useCounter;
} cache;

typedef struct {
    uint64_t tag;
    uint64_t idx;
    uint64_t offset;
} addressParts;

// DEBUG AND HELPER FUNCTIONS
void printHelp();
void printError();
void printArgs();

// CACHE SIMULATION FUNCTIONS
cache* createCache(int s, int E, int b);
void freeCache(cache* c);
addressParts parseAddress(uint64_t address, int s, int b);

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
    cache* myCache = createCache(indexBits, lineCount, offsetBits);

    // Iterate through traceFile lines

    // Deallocate cache
    freeCache(myCache);

    // Get summary for grading
    printSummary(0, 0, 0);
    return 0;
}
/*
 * Function:	getEvictLine
 * Input:	set <set_> - Set to evict a line from
 * 		int <lineCount_> - set associativity
 * Output:	int <victim> - index (0-E) of which line to evict from <set_>
 * Description:
 * Take in <set_> and <lineCount_> and iterate through each line within the set
 * to find the lowest (oldest) accessTime. The index of the line with the lowest
 * accessTime is returned to the calling function to perform the cache eviction.
 * 
 * NOTE: the index being returned IS NOT equivalent to the cache tag. The index
 * is merely where in the set the line to evict exists.
 */
int getEvictLine(set set_, int lineCount_) {
    int victim = 0;
    int lowest = set_.lines[0].accessTime;
    int i;
    for(i = 1; i < lineCount_; i++) {
    	if (set_.lines[i].accessTime < lowest) {
	    lowest = set_.lines[i].accessTime;
	    victim = i;
	}	
    }

    return i;
}

/*
 * Function:	parseAddress
 * Input:	uint64_t <address>
 * 		int <s>
 * 		int <b>
 * Output:	addressParts - addressParts struct with parsed address data
 * Description:
 * Take in an address and user defined set index bits and block offset bits
 * and parse the useful data into an addressParts struct.
 */
addressParts parseAddress(uint64_t address, int s, int b) {
    addressParts parts;

    uint64_t idxMask = (uint64_t)((1 << s) - 1);
    uint64_t offsetMask = (uint64_t)((1 << b) - 1);

    parts.offset = address & offsetMask;
    parts.idx = (address >> b) & idxMask;
    parts.tag = address >> (s + b);

    return parts;
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
 * 	valid		= false
 * 	tag		= -1
 * 	block		= 0
 * 	accessTime	= 0
 */
cache* createCache(int s, int E, int b) {
    cache* c = malloc(sizeof(cache));
    c->s = s;
    c->E = E;
    c->b = b;
    c->useCounter = 0;

    int S = 1 << s;
    c->sets = malloc((long unsigned int)S * sizeof(set));

    for (int i=0; i < S; i++) {
    	c->sets[i].lines = malloc((long unsigned int)E * sizeof(line));
	for (int j = 0; j < E; j++) {
	    c->sets[i].lines[j].valid = false;
	    c->sets[i].lines[j].tag = -1;
	    c->sets[i].lines[j].block = 0;
	    c->sets[i].lines[j].accessTime = 0;
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
