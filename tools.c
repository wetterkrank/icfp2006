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

void dumpArrays (uint arrCount, uint *arrList, uint **arr)
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

void dumpArrayToDisk (uint *arrList, uint **arr, uint id)
{
    FILE *fp = fopen("./arrdump", "wb");
    fwrite(arr[id], sizeof(uint), arrList[id], fp);
    fclose(fp);
};

void writeInstructionToFile (uint pos, int operator, int A, int B, int C, uint value, uint *regs)
{
    FILE* fp = fopen("./opdump", "a");
    if (operator == 13) fprintf(fp, "%d. op: %d, Val %d -> reg %d\n", pos, operator, value, A);
    else fprintf(fp, "%d. op: %d, reg %d: %d, reg %d: %d, reg %d: %d\n", pos, operator, A, regs[A], B, regs[B], C, regs[C]);               
    fclose(fp);
};

void listNewArray(uint *arrCount, uint **arrList, uint newArrLen)
// add 1 pointer to the list, allocate memory for it, increase arrayCount by 1
// todo: get rid of the separate list and keep the size in the struct
{
    // printf("Expanding the list; arrCount: %d\n", *arrCount);
    uint newListLen = *arrCount + 1;

    *arrList = realloc(*arrList, newListLen*sizeof(uint));
    if (*arrList == NULL) exit(2);

    (*arrList)[*arrCount] = newArrLen;
    *arrCount = *arrCount + 1;
    // printf("List expanded; new arrCount: %d\n", *arrCount);
};

uint addArray(uint *arrCount, uint **arrList, uint ***arr, uint newArrLen)
{
    //printf("\nAdding an array; arrCount: %d, newArrLen: %d\n", *arrCount, newArrLen);

    (*arr) = realloc(*arr, (*arrCount+1)*sizeof(*arr)); // add memory for the new pointer
    if (*arr == NULL) exit(2);

    (*arr)[*arrCount] = calloc(newArrLen, sizeof(uint)); // add memory for the data it's pointing at
    if ((*arr)[*arrCount] == NULL) exit(2);

    listNewArray(arrCount, arrList, newArrLen);
    // printf("New array #%u, size in the list %u\n", *arrCount-1, (*arrList)[*arrCount-1]);

    return *arrCount-1; // todo: reuse old slots
};

void loadArray(uint *arrCount, uint **arrList, uint ***arr, uint srcID)
{
    (*arr)[0] = realloc((*arr)[0], (*arrList)[srcID]*sizeof(uint));
    if ((*arr)[0] == NULL) exit(2);

    (*arrList)[0] = (*arrList)[srcID];
    memcpy((*arr)[0], (*arr)[srcID], (*arrList)[srcID]*sizeof(uint));

    // printf("Copied array %u to array %u\n", srcID, 0);
};

void dropArray(uint *arrCount, uint **arrList, uint ***arr, uint id)
// todo: actually downsize arr when dropping an array?
{
    // printf("Removing array; arrCount: %d, id to remove: %d\n", *arrCount, id);
    free((*arr)[id]);
    (*arrList)[id] = 0;
};
