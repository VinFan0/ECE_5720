// this file defines the constants for the virtual memory lab
// it will be replaced to test for different sizes, etc
// ALL of the constants in this file may be changed by the grader
// DO NOT add to this file
//
// This is the example constants file with the same prameters as 9.6.4 from the textbook

// the width of the virtual address (14 in example)
#define VIRTUAL_WIDTH 14
// the width of the physical address (12 in example)
#define PHYSICAL_WIDTH 12
// the page size in bytes (64 in the example)
#define PAGE_SIZE 64 
// the TLB associativity (4 in the example)
#define TLB_ASSOC 4
// the TLB size (16 in the example)
#define TLB_SIZE 16

//define a tlb entry struct
struct tlbEntry {
  int tag;
  int ppn;
  bool valid = false;
};

// our tlb is a 2d array of tlbEntries
extern struct tlbEntry tlb[TLB_SIZE/TLB_ASSOC][TLB_ASSOC];

// page table
extern int pageTable[16];
extern bool pageTableValid[16];

// function in constants.cpp that sets up tlb and page table
void setupConstants();
