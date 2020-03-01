#ifndef TOOLS
#define TOOLS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned uint;

void bitPrint (uint num);
void dumpArrays (uint arrCount, uint *arrList, uint **arr);
void dumpArrayToDisk (uint* arrList, uint** arr, uint id);
void writeInstructionToFile (uint pos, int operator, int A, int B, int C, uint value, uint* regs);

void listNewArray(uint* arrCount, uint** arrList, uint newArrLen);
uint addArray(uint* arrCount, uint** arrList, uint*** arr, uint newArrLen);
void loadArray(uint* arrCount, uint** arrList, uint*** arr, uint srcID);
void dropArray(uint* arrCount, uint** arrList, uint*** arr, uint id);

#endif