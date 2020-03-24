#include "tools.h"

void dumpArray (struct array **arr, uint id)
{
    printf("arr %d (len %d): \n", id, arr[id]->length);
    for (int j=0; j<arr[id]->length; j++)
    {
        printf("%x\n", arr[id]->cells[j]);
    }
    printf("\n");
};

uint addArray(uint *arrCount, struct array ***arr, uint newArrLen)
{
    // add memory for the new pointer or reuse the old one
    uint newId;
    if (!freeId) {
        newId = *arrCount;
        *arr = realloc(*arr, (*arrCount+1)*sizeof(struct array **)); 
        if (*arr == NULL) exit(2);
        *arrCount = *arrCount + 1;    
    } else {
        newId = freeId;
        freeId = (uintptr_t)(*arr)[newId]; // take the next free ids
    }

    // add memory for the struct array and copy
    (*arr)[newId] = malloc(sizeof(struct array)+newArrLen*sizeof(uint));
    if ((*arr)[newId] == NULL) exit(2);
    memset((*arr)[newId], 0, sizeof(struct array)+newArrLen*sizeof(uint));
    (*arr)[newId]->length = newArrLen;

    return newId;
};

void loadArray(uint *arrCount, struct array ***arr, uint srcID)
{
    free((*arr)[0]);
    (*arr)[0] = malloc(sizeof(struct array)+(*arr)[srcID]->length*sizeof(uint));
    if ((*arr)[0] == NULL) exit(2);
    // copy the whole struct over
    memcpy((*arr)[0], (*arr)[srcID], sizeof(struct array)+(*arr)[srcID]->length*sizeof(uint));
};

void dropArray(uint *arrCount, struct array ***arr, uint id)
{
    free((*arr)[id]);
    (*arr)[id] = (struct array *)freeId; // store the free array id in place of an unused pointer
    freeId = id;
};
