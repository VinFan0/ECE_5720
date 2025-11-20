
#include<stdio.h> //printf
#include<math.h>  //log2
#include<stdlib.h>//strtol

#include "constants.h"

// pretty print TLB, included for help with debugging
void printTLB() {
  printf("TLB: \n");
  // print headers
  printf("%4s|", "Set");
  for (int set = 0; set < TLB_ASSOC; set++)
    printf("%3s %4s %5s|", "Tag", "PPN", "Valid");
  // print set
  printf("\n");
  for (int set = 0; set < TLB_SIZE/TLB_ASSOC; set++) {
    printf("%3d |", set);
    // print entries
    for (int entry = 0; entry < TLB_ASSOC; entry++) {
      printf(" %02x   %02x  %3d |", tlb[set][entry].tag, 
          tlb[set][entry].ppn, tlb[set][entry].valid);
    }
    printf("\n");
  }
  printf("\n");
}

// print page table, can be used in debugging if desired
void printPageTable() {
  printf("Page Table:\nVPN  PPN Valid\n");
  for (int i = 0; i < 16; i++) {
    printf(" %02x | %02x %1d |\n", i, pageTable[i], pageTableValid[i]);
  }
  printf("\n");
}

// print int x as binary representation, divided into nybbles
void print_hex_as_bin(int x, int length) {
  for (int i = length; i >= 0; i--) {
    putchar((x & (1u << i)) ? '1' : '0');
    if (i % 4 == 0 && i != 0) putchar(' ');
  }
  putchar('\n');
}

int searchTLB(int set, int tag) {
  int result = 0;

  printTLB();

  for (int i=0; i<TLB_ASSOC; i++) {
    if (tlb[set][i].tag == tag && tlb[set][i].valid) {
      result = true;
      // DEBUG printf("TLB HIT at set %d tag %x\n", set, tag);
      printf("TLB HIT\n");
      result = tlb[set][i].ppn;
      return result;
    }
  }

  printf("TLB MISS\n");
  return result;
}

int searchPT(int vpn) {

  printPageTable();

  if(pageTableValid[vpn]) {
    //printf("Found in pt\n");
    return pageTable[vpn];
  } else {
    printf("PAGE FAULT\n");
    return 0;
  }

}

// translates virtual address to physical address
int translate(int virtualAddress) {
  // TODO: implement address translation (insert code here)
  int physicalAddress = 0;

  int page_offset_bits = log2(PAGE_SIZE);
  //int page_offset_mask = (1<<page_offset_bits) - 1;

  int virtual_page_number = virtualAddress >> page_offset_bits;

  int tlb_set_number = TLB_SIZE / TLB_ASSOC;
  int tlb_set_bits = log2(tlb_set_number);
  int tlb_set_mask = (1<<tlb_set_bits) - 1;

  int tlb_set = virtual_page_number & tlb_set_mask;
  int tlb_tag = virtual_page_number >> tlb_set_bits;

  int tlb_entry= searchTLB(tlb_set, tlb_tag);
  if (tlb_entry != 0) {
    physicalAddress = tlb_entry;
  } else {
    int pt_entry = searchPT(virtual_page_number);
    if (pt_entry != 0) {
      physicalAddress = pt_entry;
    }
  }

  // DEBUG printing stuff
  
  printf("TLB Details\n");
  printf("Page size: %d\n", PAGE_SIZE);
  printf("VPO size: %d\n", page_offset_bits);
  printf("VPN size: %d\n", VIRTUAL_WIDTH-page_offset_bits);
  printf("Set bits: %d\n", tlb_set_bits);
  printf("\n");

  printf("Address: %x\n", virtualAddress);
  print_hex_as_bin(virtualAddress, VIRTUAL_WIDTH);
  printf("Virtual page number: ");
  print_hex_as_bin(virtual_page_number, page_offset_bits); 
  printf("Set: %d\n", tlb_set);
  printf("Tag: %x\n", virtual_page_number >> tlb_set_bits);
  

  return physicalAddress;
}

// main function
int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("requries virtual address in HEX as argument!\n");
    return 0;
  }
  // set up TLB and page table (in constants.cpp)
  setupConstants();

  //take input address and parse it as a hexadecimal number
  int inputAddress = (int)strtol(argv[1], NULL, 16);

  //translate to a physical address and print results
  printf("Physical address: %x \n", translate(inputAddress));

  return 0;
}
