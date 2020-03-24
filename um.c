// Universal Machine for ICFP 2006
// Task: http://boundvariable.org/task.shtml

#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main (int args, const char *av[])
{
    if (args != 2) { printf("usage: um.exe <program.um>\n"); exit(1); }

    // get UM file size; note: no error handling for file ops
    FILE *fp = fopen(av[1], "rb");
    fseek(fp, 0L, SEEK_END);
    uint fileSize = ftell(fp); // todo: check if it's the factor of size(uint)
    rewind(fp);

    // init controls
    struct array **arr;
    arr = malloc(sizeof(struct array **)); if (arr == NULL) exit(2);
    arr[0] = malloc(sizeof(struct array)+fileSize); if (arr[0] == NULL) exit(2);
    arr[0]->length = fileSize/sizeof(uint);

    // read the file into the 0 array and convert to big endian
    fread(arr[0]->cells, sizeof(uint), arr[0]->length, fp);
    fclose(fp);
    for (uint i = 0; i < arr[0]->length; i++) arr[0]->cells[i] = htonl(arr[0]->cells[i]);

    uint arrCount = 1; // max number of arrays we have
    uint regs[8] = {0};
    uint pos = 0; // instruction to execute

    char operator, A, B, C;
    char ch;
    uint instruction;

    for (; pos < arr[0]->length; pos++)
    {
        instruction = arr[0]->cells[pos];
        operator = (instruction >> 28) & 15;
        C = instruction & 7;
        B = (instruction >> 3) & 7;
        A = (instruction >> 6) & 7;

        //if (operator == 13) printf("%d. op: %d, Val %d -> reg %d\n", pos, operator, instruction & 0x01FFFFFF, A);
        //else printf("%d. op: %d, reg %d: %d, reg %d: %d, reg %d: %d\n", pos, operator, A, regs[A], B, regs[B], C, regs[C]);               

        switch (operator) 
        {
            case 0:
                if (regs[C] != 0) regs[A] = regs[B];
                break;
            case 1:
                regs[A] = arr[regs[B]]->cells[regs[C]];
                break;
            case 2:
                arr[regs[A]]->cells[regs[B]] = regs[C];
                break;
            case 3:
                regs[A] = regs[B] + regs[C];
                break;
            case 4:
                regs[A] = regs[B] * regs[C];
                break;
            case 5:
                regs[A] = regs[B] / regs[C];
                break;
            case 6:
                regs[A] = ~(regs[B] & regs[C]);
                break;
            case 7:
                exit(0);
            case 8:
                regs[B] = addArray(&arrCount, &arr, regs[C]); 
                break;
            case 9:
                dropArray(&arrCount, &arr, regs[C]);
                break;
            case 10:
                printf("%c", regs[C]);
                break;
            case 11:
                ch = fgetc (stdin);
                if (EOF == ch) 
                    regs[C] = 0xFFFFFFFF;
                else 
                    regs[C] = ch;
                break;
            case 12:
                if (regs[B] != 0) 
                    loadArray(&arrCount, &arr, regs[B]);
                pos = regs[C]-1; // pos++ at the next loop
                break;
            case 13:
                regs[(instruction >> 25) & 7] = instruction & 0x01FFFFFF; // bits 26-28 and 1-25
                break;
            default:
                printf("unknown instruction (%d); machine halted\n", operator);
                exit(1);
        };
    };
    return 0;
};