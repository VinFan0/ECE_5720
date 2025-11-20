
#include "constants.h"

// define the page table and page table valid arrays
int pageTable[] = {
  0x28, 0, 0x33, 0x02, 0, 0x16, 0, 0, 
  0x13, 0x17, 0x09, 0, 0, 0x2D, 0x11, 0x0D};
bool pageTableValid[] = {
  1, 0, 1, 1, 0, 1, 0, 0,
  1, 1, 1, 0, 0, 1, 1, 1};


struct tlbEntry tlb[TLB_SIZE/TLB_ASSOC][TLB_ASSOC]; 


void setupConstants() {
  //add tlb entries manually for now
  tlb[0][1].tag = 0x09;
  tlb[0][1].ppn = 0x0D;
  tlb[0][1].valid = true;
  //tlb[0][3] = {0x07, 0x02, true};
  //tlb[1][0] = {0x03, 0x2D, true};
  tlb[3][1].tag = 0x03;
  tlb[3][1].ppn = 0x0D;
  tlb[3][1].valid = true;
  //tlb[3][2] = {0x0A, 0x34, true};
 
}
