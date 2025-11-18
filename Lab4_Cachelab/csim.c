/*
 * Cache Lab
 * ECE 5720
 * Ryan Beck A02237765
 * Josh Christensen A02375004
 */
#include "cachelab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>

// CACHE SIMULATION VARIABLES AND TYPES
int indexBits;
int lineCount;
int offsetBits;
char traceFile[32];
int hits = 0;
int misses = 0;
int evictions = 0;
bool verboseOutput = false;

typedef struct {
    bool valid;
    int tag;
    unsigned long long block;
    unsigned long long accessTime;
} line;

typedef struct {
    line* lines;
} set;

typedef struct {
    set* sets;
    int s;
    int E;
    int b;
    unsigned long long accessCounter;
} cache;

typedef struct {
    int tag;
    int idx;
    int offset;
} addressParts;

// DEBUG AND HELPER FUNCTIONS
void printHelp();
void printError();
void printArgs();

// CACHE SIMULATION FUNCTIONS
cache* createCache(int s, int E, int b);
void freeCache(cache* c);
addressParts parseAddress(unsigned long long address, int s, int b);
int getEvictLine(set *set_);
void runTrace(cache *c);
void retrieveCacheLine(cache *c, unsigned long long addr);

// MAIN FUNCTION CODE
int main(int argc, char* argv[])
{
    // Flag check
    // Required flags: s, E, b, t
    // Optional flags: h
    int opt;
    int sFlag = 0, eFlag = 0, bFlag = 0, tFlag = 0;

    while((opt = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
    	switch (opt) {
	    case 'h':
		printHelp();
		return 0;

	    case 'v':
		verboseOutput = true;
		break;

	    case 's':
		indexBits = atoi(optarg);
		sFlag = 1;
		break;

	    case 'E':
		lineCount = atoi(optarg);
		eFlag = 1;
		break;

	    case 'b':
		offsetBits = atoi(optarg);
		bFlag = 1;
		break;

	    case 't':
		strncpy(traceFile, optarg, sizeof(traceFile)-1);
		traceFile[sizeof(traceFile)-1] = '\0';
		tFlag = 1;
		break;

	    default:
		printError();
		printHelp();
		return 1;
	}
    }

    if (!sFlag || !eFlag || !bFlag || !tFlag) {
    	printError();
	printHelp();
	return 1;
    }

   
    // DEBUG: display input arguments
    printArgs();

    // Generate Cache Table
    cache* myCache = createCache(indexBits, lineCount, offsetBits);

    // Run trace
    runTrace(myCache);   

    // WRAP UP PROCESS
    // Deallocate cache
    freeCache(myCache);

    // Get summary for grading
    printSummary(hits, misses, evictions);
    return 0;
}
/*
 * Function:	runTrace
 * Input:	cache *<c> - dynamically allocated cache
 * 		char * <traceFile> - User input trace file
 * Output:	void
 * Description:
 * Iterate through lines of the tracefile, searching for non-instruction load
 * operations. Any other operations (Load (L), Store (S), Modify (M)) will
 * generate an access to the cache. Modify instrucitons incur a second access,
 * as they are essentially a Load+Store pair.
 */
void runTrace(cache *c) {
    FILE *fp = fopen(traceFile, "r");
    if (!fp) {
    	printf("ERROR: cannot open trace file %s\n", traceFile);
	exit(1);
    }

    char operation;
    unsigned long long addr;
    int size;

    while (fscanf(fp, " %c %llx,%d", &operation, &addr, &size) == 3) {
    	if (operation == 'I')
	    continue;

	if (verboseOutput) printf("%c %llx,%d", operation, addr, size);
	switch (operation) {
	    case 'L':
	    case 'S':
		retrieveCacheLine(c, addr);
		break;

	    case 'M':
		retrieveCacheLine(c, addr);
		retrieveCacheLine(c, addr);
		break;

	    default:
		printf("Read something weird: %c\n", operation);
		break;

	}
	if (verboseOutput) printf("\n");
    }

    fclose(fp);
}


/*
 * Function:	retrieveCacheLine
 * Input:	cache *<c>
 * 		unsigned long long <addr>
 * Output:	void
 * Description:
 * Take in the cache <c> with address to access <addr> and parse <addr> into
 * an addressParts struct using parseAddress. Increment the LRU accessCounter.
 * With the parsed data, search the requested set for a matching tag and valid bit.
 * If yes, 
 * 	increment hits, update accessTime for that line, and return. 
 *
 * Else, 
 * 	increment misses and search for an empty line (invalid bit) in the set to
 * 	insert the accessed line into. 
 * 	If successful,
 * 		update the cache line with address data, including valid bit,
 *		tag, and accessTime and return
 *	Else, (no free lines)
 *		Increment evictions and call getEvictLine to find oldest entry in
 *		the set. Replace the found line with the accessed line.
 */
void retrieveCacheLine(cache *c, unsigned long long addr) {
    addressParts parts = parseAddress(addr, indexBits, offsetBits);
    set * curSet = &c->sets[parts.idx];
    c->accessCounter++;

    for (int i = 0; i < lineCount; i++) {
    	line *line_ = &curSet->lines[i];
	if (line_->valid && line_->tag == parts.tag) {
	    if (verboseOutput) printf(" hit");
	    hits++;
	    line_->accessTime = c->accessCounter;
	    return;
	}
    }

    if (verboseOutput) printf(" miss");
    misses++;

    for (int i = 0; i < lineCount; i++) {
    	line *line_ = &curSet->lines[i];
	if (!line_->valid) {
	    line_->valid = true;
	    line_->tag = parts.tag;
	    line_->accessTime = c->accessCounter;
	    return;
	}
    }

    if (verboseOutput) printf(" eviction");
    evictions++;

    int victimIndex = getEvictLine(curSet);
    line *victimLine = &curSet->lines[victimIndex];
    victimLine->tag = parts.tag;
    victimLine->accessTime = c->accessCounter;
    victimLine->valid = true;
}


/*
 * Function:	getEvictLine
 * Input:	set *<set_> - Set to evict a line from 
 * Output:	int <victim> - index (0-E) of which line to evict from <set_>
 * Description:
 * Take in <set_> and and iterate through each line within the set
 * to find the lowest (oldest) accessTime. The index of the line with the lowest
 * accessTime is returned to the calling function to perform the cache eviction.
 * 
 * NOTE: the index being returned IS NOT equivalent to the cache tag. The index
 * is merely where in the set the line to evict exists.
 */
int getEvictLine(set *set_) {
    int victim = 0;
    unsigned long long lowest = set_->lines[0].accessTime;
    
    for(int i = 1; i < lineCount; i++) {
    	if (set_->lines[i].accessTime < lowest) {
	    lowest = set_->lines[i].accessTime;
	    victim = i;
	}	
    }

    return victim;
}

/*
 * Function:	parseAddress
 * Input:	unsigned long long <address>
 * 		int <s>
 * 		int <b>
 * Output:	addressParts - addressParts struct with parsed address data
 * Description:
 * Take in an address and user defined set index bits and block offset bits
 * and parse the useful data into an addressParts struct.
 */
addressParts parseAddress(unsigned long long address, int s, int b) {
    addressParts parts;

    unsigned long long idxMask = (unsigned long long)((1 << s) - 1);
    unsigned long long offsetMask = (unsigned long long)((1 << b) - 1);

    parts.offset = (int) (address & offsetMask);
    parts.idx = (int) ((address >> b) & idxMask);
    parts.tag = (int) (address >> (s + b));

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
    c->accessCounter = 0;

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
