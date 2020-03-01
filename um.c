// Task: http://boundvariable.org/task.shtml

#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

typedef unsigned uint;

void getOperator(uint instruction, int* op, int* A, int* B, int* C, uint* V)
{
    *op = (instruction >> 28) & 15; // bits 28-32
    if (*op != 13) {
        *C = instruction & 7; // bits 1-3
        *B = (instruction >> 3) & 7; // bits 4-6
        *A = (instruction >> 6) & 7; // bits 7-9
    }
    else {
        *A = (instruction >> 25) & 7; // bits 26-28
        *V = (instruction & 0x01FFFFFF); // bits 1-25
    };
};

int main (void) {

    // get UM file size; note: no error handling for file ops
    FILE* fp = fopen("./sandmark.umz", "rb");
    fseek(fp, 0L, SEEK_END);
    uint fileSize = ftell(fp); // todo: check if it's the factor of size(uint)
    rewind(fp);

    // init controls
    uint* arrList; // to keep the array lengths
    uint** arr; // a kind of jagged array of arrays
    
    arr = malloc(1*sizeof(arr)); if (arr == NULL) exit(1);
    arr[0] = malloc(fileSize); if (arr[0] == NULL) exit(1);
    arrList = calloc(1, sizeof(arrList)); if (arrList == NULL) exit(1);
    arrList[0] = fileSize/sizeof(uint);

    // read the file into the 0 array and convert to big endian
    fread(arr[0], sizeof(uint), arrList[0], fp);
    fclose(fp);
    for (uint i = 0; i < arrList[0]; i++) arr[0][i] = htonl(arr[0][i]);

    uint arrCount = 1; // number of arrays we have
    uint regs[8] = {0};
    uint pos = 0; // instruction to execute

    uint value;
    int operator, A, B, C;

    for (; pos < arrList[0]; pos++)
    {
        uint instruction = arr[0][pos];
        getOperator(instruction, &operator, &A, &B, &C, &value);

        //if (operator == 13) printf("%d. op: %d, Val %d -> reg %d\n", pos, operator, value, A);
        //else printf("%d. op: %d, reg %d: %d, reg %d: %d, reg %d: %d\n", pos, operator, A, regs[A], B, regs[B], C, regs[C]);               

        switch (operator) 
        {
            case 0:
                if (regs[C] != 0) regs[A] = regs[B];
                break;
            case 1:
                regs[A] = arr[regs[B]][regs[C]];
                break;
            case 2:
                arr[regs[A]][regs[B]] = regs[C];
                break;
            case 3:
                regs[A] = (regs[B] + regs[C]) & 0xFFFFFFFF; // aka % 0x100000000
                break;
            case 4:
                regs[A] = (regs[B] * regs[C]) & 0xFFFFFFFF;
                break;
            case 5:
                regs[A] = regs[B] / regs[C];
                break;
            case 6:
                regs[A] = ~regs[B] | ~regs[C];
                break;
            case 7:
                exit(0);
            case 8:
                regs[B] = addArray(&arrCount, &arrList, &arr, regs[C]); 
                break;
            case 9:
                dropArray(&arrCount, &arrList, &arr, regs[C]);
                break;
            case 10:
                printf("%c", regs[C]);
                break;
            case 11:
                break;
            case 12:
                // printf("%d: Load array %d, run position %d\n", pos, regs[B], regs[C]);
                if (regs[B] != 0) 
                    loadArray(&arrCount, &arrList, &arr, regs[B]);
                pos = regs[C]-1; // pos++ at the next loop
                break;
            case 13:
                regs[A] = value;
        };
    };

    return 0;
};