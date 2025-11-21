
#include "constants.h"

struct tlbEntry tlb[TLB_SIZE/TLB_ASSOC][TLB_ASSOC]; 
int pageTable[1024];
bool pageTableValid[1024];


void setupConstants() {

  //add tlb entries manually for now
  tlb[0][7].tag = 0x7A;
  tlb[0][7].ppn = 0x19;
  tlb[0][7].valid = true;
  
  //add page table entries
  pageTable[0xF4] = 0x19;
  pageTableValid[0xF4] = true;
}
