// A 32-bit UM version using the standard memory allocation routines.
// Runs sandmark.umz in less than 1 minute. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef uintptr_t uint;

uint myByteSwap(uint num)
{
    uint b0,b1,b2,b3;
    uint res = 0;
    b0 = (num & 0x000000ff) << 24u;
    b1 = (num & 0x0000ff00) << 8u;
    b2 = (num & 0x00ff0000) >> 8u;
    b3 = (num & 0xff000000) >> 24u;
    res = b0 | b1 | b2 | b3;
    return res;
}

void die(const char *message)
{
    if (errno) perror(message);
    else printf("error: %s\n", message);
    exit(1);
}

int main (int args, const char *av[]) 
{
    setbuf(stdout, NULL);
    if (args != 2) die("usage: um.exe <program.um>");

    // get UM file size; still no error handling for file ops
    FILE *fp = fopen(av[1], "rb");
    fseek(fp, 0L, SEEK_END);
    uint fileSize = ftell(fp);
    uint progLen = fileSize/4; // todo: check if it's the factor of size(uint)
    rewind(fp);

    // init controls
    uint *zero;
    zero = malloc(fileSize+sizeof(uint)); 
    if (zero == NULL) die("program array memory allocation failed");
    fread(zero+1, sizeof(uint), progLen, fp);
    fclose(fp);
    for (uint i = 1; i <= progLen; i++) zero[i] = myByteSwap(zero[i]); //note: <= progLen
    zero[0] = progLen; //todo: move to struct

    uint regs[8] = {0};
    uint pos = 1; // skipping 0 cell used to keep the program size

    char operator, A, B, C;
    uint instruction;

    for (; pos <= progLen; pos++)
    {
        instruction = zero[pos];
        operator = (instruction >> 28) & 15;
        C = instruction & 7;
        B = (instruction >> 3) & 7;
        A = (instruction >> 6) & 7;
        uint *arr; //tmp

        //if (operator == 13) printf("%d. op: %d, Val %d -> reg %d\n", pos-1, operator, instruction & 0x01FFFFFF, A);
        //else printf("%d. op: %d, reg %d: %d, reg %d: %d, reg %d: %d\n", pos-1, operator, A, regs[A], B, regs[B], C, regs[C]);

        switch (operator) 
        {
            case 0:
                if (regs[C] != 0) regs[A] = regs[B];
                break;
            case 1:
                if (regs[B] != 0) arr = (uint *)regs[B]; else arr = zero;
                regs[A] = arr[regs[C]+1];
                break;
            case 2:
                if (regs[A] != 0) arr = (uint *)regs[A]; else arr = zero;
                arr[(regs[B]+1)] = regs[C];
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
                printf("machine halted\n");
                exit(0);
            case 8: {
                arr = calloc(regs[C]+1, sizeof(uint));
                if (arr == NULL) die("new array memory allocation failed");
                arr[0] = regs[C];
                regs[B] = (uint)arr;
                break; }
            case 9:
                free((uint *)regs[C]);
                break;
            case 10:
                printf("%c", regs[C]);
                break;
            case 11: {
                fflush(stdout);
                char ch = fgetc (stdin);
                if (EOF == ch) regs[C] = 0xFFFFFFFF; else regs[C] = ch;
                break; }
            case 12:
                if (regs[B] != 0) {
                    free(zero);
                    arr = (uint *)regs[B];
                    progLen = arr[0];
                    zero = malloc((progLen+1)*sizeof(uint));
                    if (zero == NULL) die("program array memory allocation failed");
                    memcpy(zero, arr, (progLen+1)*sizeof(uint)); }
                pos = regs[C]; // pos will ++ at the next loop
                break;
            case 13:
                regs[(instruction >> 25) & 7] = instruction & 0x01FFFFFF; // bits 26-28 and 1-25
        };
    };

    printf("end of program\n");
    return 0;
};