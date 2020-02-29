// Task: http://boundvariable.org/task.shtml

#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

typedef unsigned uint;

int getOperator(uint instruction, int* op, int* A, int* B, int* C, uint* V)
{
    *op = (instruction & 0xF0000000) >> 28; // bits 28-32
    if (*op != 13) {
        *C = instruction & 7; // bits 1-3
        *B = (instruction >> 3) & 7; // bits 4-6
        *A = (instruction >> 6) & 7; // bits 7-9
    }
    else {
        *A = (instruction >> 25) & 7; // bits 26-28
        *V = (instruction & 0x01FFFFFF); // bits 1-25
    };
    return 0;
};

int main (void) {

    // get UM file size; note: no error handling for file ops
    FILE* fp = fopen("./sandmark.umz", "rb");
    fseek(fp, 0L, SEEK_END);
    uint fsize = ftell(fp); // todo: check if it's the factor of size(uint)
    rewind(fp);

    // init controls
    uint arrCount = 0; // number of arrays we have
    uint* arrList; // to keep the array lengths
    uint** arr; // a kind of jagged array of arrays
    
    addArray(&arrCount, &arrList, &arr, fsize/sizeof(uint));

    uint regs[8] = {0};
    uint progID = 0; // array to execute
    uint pos = 0; // instruction to execute

    // read the file into the 0 array
    fread(arr[0], sizeof(uint), fsize/sizeof(uint), fp);
    fclose(fp); // todo: how about htonl the whole array once here?

    for (uint i = 0; i < fsize/sizeof(uint); i++) {
        arr[0][i] = htonl(arr[0][i]);
    };

    do
    {

        uint value;
        int operator, A, B, C;

        // loop through the instructions
        for (; pos < arrList[progID]; pos++)
        {
            uint instruction = arr[progID][pos];
            getOperator(instruction, &operator, &A, &B, &C, &value);

            // if (operator == 13) printf("%d. op: %d, Val %d -> reg %d\n", pos, operator, value, A);
            // else printf("%d. op: %d, reg %d: %d, reg %d: %d, reg %d: %d\n", pos, operator, A, regs[A], B, regs[B], C, regs[C]);               

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
                    regs[A] = (regs[B] + regs[C]); // & 0xFFFFFFF aka % 0x100000000, to avoid overflow?
                    break;
                case 4:
                    regs[A] = (regs[B] * regs[C]); // & 0xFFFFFFF;
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
                    // printf("%d: Execute array %d, position %d\n", pos, regs[B], regs[C]);
                    if (regs[B] != 0)
                    {
                        copyArray(&arrCount, &arrList, &arr, regs[B], 0);
                        // printf("Switching to array %u, pos %u\n", regs[B], regs[C]);
                    };
                    pos = regs[C];
                    break;
                case 13:
                    regs[A] = value;
            };

            if (operator == 12) break; // restart the spin with the selected progID and pos

        }; // execution loop
    }
    while (pos < arrList[progID]);

    return 0;
};