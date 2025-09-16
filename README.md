# ECE_5720

## LAB 1
**Running**
Provided in lab 1, the bomb lab, is a `.tar` file contains 2 files. 
- `bomb` binary executable. This is the program to be run using gdb.
- `bomb.c` limited C code. This file contains the main function for the lab and general structure of the program. 

The `bomb` executable must be run from a linux machine in either room 103 or 105 of the ENLAB building. Transfer the `.tar` file to the computer, extract it there, then it can be run.

**Notes**
The `bomb` executable allows for a single argument being the name of a `.txt` file containing pass phrases you desire to run. This can be used in multiple ways
- Run from terminal with `bomb <file.txt>`
- Run every time with gdb with `gdb --args bomb <file.txt>`
- Run selectively from gdb by running `gdb bomb`, then `run <file.txt>` inside gdb.
