#ifndef CONSTANTS
#define CONSTANTS

#define DEBUG

// this file defines the constants for the virtual memory lab
// it will be replaced to test for different sizes, etc
// ALL of the constants in this file may be changed by the grader
// DO NOT add to this file
//
// This is the constants2 file with a original stuff

// the width of the virtual address
#define VIRTUAL_WIDTH 20
// the width of the physical address 
#define PHYSICAL_WIDTH 15
// the page size in bytes (64 in the example)
#define PAGE_SIZE 1024 
// the TLB associativity (4 in the example)
#define TLB_ASSOC 8
// the TLB size (16 in the example)
#define TLB_SIZE 16

// define the page table and page table valid arrays
extern int pageTable[1024];
extern bool pageTableValid[1024];

//define a tlb entry struct
struct tlbEntry {
  int tag;
  int ppn;
  bool valid = false;
};

// our tlb is a 2d array
extern struct tlbEntry tlb[TLB_SIZE/TLB_ASSOC][TLB_ASSOC]; 

void setupConstants();

#endif
