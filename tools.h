#ifndef TOOLS
#define TOOLS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned uint;
struct array {
    uint length;
    uint cells[0];
};

static uintptr_t freeId = 0;

void dumpArray (struct array **arr, uint id);

uint addArray(uint* arrCount, struct array ***arr, uint newArrLen);
void loadArray(uint* arrCount, struct array ***arr, uint srcID);
void dropArray(uint* arrCount, struct array ***arr, uint id);

#endif