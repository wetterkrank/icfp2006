#include "tools.h"

void bitPrint (unsigned num)
{
  int size = sizeof(unsigned);
  unsigned maxPow = 1 << (size*8-1);
  int pos=0;
  for(; pos<size*8; ++pos)
  {
      printf("%u", num&maxPow ? 1 : 0);
      num = num<<1;
  }
  printf("\n");
};

void dumpArrays (uint arrCount, uint* arrList, uint** arr)
{
    for (int i=0; i<arrCount; i++)
    {
        printf("%d (%d): ", i, arrList[i]);
        for (int j=0; j<arrList[i]; j++)
        {
            printf("%d", arr[i][j]);
        }
        printf("\n");
    }
};

void dumpArrayToDisk (uint* arrList, uint** arr, uint id)
{
    FILE* fp = fopen("./arrdump", "wb");
    fwrite(arr[id], sizeof(uint), arrList[id], fp);
    fclose(fp);
};

void listNewArray(uint* arrCount, uint** arrList, uint newArrLen)
// add 1 pointer to the list, allocate memory for it, increase arrayCount by 1
// todo: get rid of the separate list and keep the size in the struct
{
    // printf("Expanding the list; arrCount: %d\n", *arrCount);
    uint newListLen = *arrCount + 1;

    if (*arrCount == 0) // todo: get rid of the 0 case to speed up
    {
        *arrList = calloc(newListLen, sizeof(uint));
        if (*arrList == NULL) exit(2);
    }
    else
    {
        *arrList = realloc(*arrList, newListLen*sizeof(uint));
        if (*arrList == NULL) exit(2);
    }

    (*arrList)[*arrCount] = newArrLen;
    *arrCount = *arrCount + 1;
    // printf("List expanded; new arrCount: %d\n", *arrCount);
};

uint addArray(uint* arrCount, uint** arrList, uint*** arr, uint newArrLen)
{
    // printf("\nAdding an array; arrCount: %d, newArrLen: %d\n", *arrCount, newArrLen);

    if (*arrCount == 0) // todo: get rid of the 0 case to speed up
    {
        *arr = malloc(1*sizeof(*arr));
        if (*arr == NULL) exit(2);
    }
    else
    {
        (*arr) = realloc(*arr, (*arrCount+1)*sizeof(*arr));
        // printf("Allocated for **arr: %d\n", sizeof(*arr));
    };

    (*arr)[*arrCount] = calloc(newArrLen, sizeof(uint));
    if ((*arr)[*arrCount] == NULL) exit(2);

    listNewArray(arrCount, arrList, newArrLen);
    // printf("New array #%u, size in the list %u\n", *arrCount-1, (*arrList)[*arrCount-1]);

    return *arrCount-1; // todo: reuse old slots
};

void copyArray(uint* arrCount, uint** arrList, uint*** arr, uint srcID, uint destID)
{
    (*arr)[destID] = realloc((*arr)[destID], (*arrList)[srcID]*sizeof(uint));
    if ((*arr)[destID] == NULL) exit(2);

    (*arrList)[destID] = (*arrList)[srcID];
    memcpy((*arr)[destID], (*arr)[srcID], (*arrList)[srcID]*sizeof(uint));

    // printf("Copied array %u to array %u\n", srcID, destID);
};

void dropArray(uint* arrCount, uint** arrList, uint*** arr, uint id)
// todo: actually downsize arr when dropping an array?
{
    // printf("Removing array; arrCount: %d, id to remove: %d\n", *arrCount, id);
    free((*arr)[id]);
    (*arrList)[id] = 0;
};

