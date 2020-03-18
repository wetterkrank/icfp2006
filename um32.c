// A 32-bit UM version using the standard memory allocation routines.
// Runs sandmark.umz in less than 1 minute.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// a pointer-sized integer and its short name
typedef uintptr_t uint;

struct array {
    uint length;
    uint elements[0]; // not the same as *elements!
};

// endianness converter
uint myByteSwap(uint num)
{
    uint b0,b1,b2,b3;
    b0 = (num & 0x000000ff) << 24u;
    b1 = (num & 0x0000ff00) << 8u;
    b2 = (num & 0x00ff0000) >> 8u;
    b3 = (num & 0xff000000) >> 24u;
    return (b0 | b1 | b2 | b3);
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
    FILE *file = fopen(av[1], "rb");
    fseek(file, 0L, SEEK_END);
    uint fileSize = ftell(file);
    if (fileSize % 4 != 0) die("invalid file size");
    uint progLen = fileSize/4;
    rewind(file);

    // read UM program into the "0" array and convert to big endian
    struct array *zero;
    zero = malloc(sizeof(*zero)+progLen*sizeof(uint));
    if (zero == NULL) die("program array memory allocation failed");
    fread(zero->elements, sizeof(uint), progLen, file);
    fclose(file);
    for (uint i = 0; i < progLen; i++)
        zero->elements[i] = myByteSwap(zero->elements[i]);
    zero->length = progLen;

    uint regs[8] = {0};
    uint pos = 0;

    char operator, A, B, C;
    uint instruction;

    for (; pos < progLen; pos++)
    {
        instruction = zero->elements[pos];
        operator = (instruction >> 28) & 15;
        C = instruction & 7;
        B = (instruction >> 3) & 7;
        A = (instruction >> 6) & 7;

        struct array *arr; // temporary pointer

        //if (operator == 13) printf("%d. op: %d, Val %d -> reg %d\n", pos, operator, instruction & 0x01FFFFFF, A);
        //else printf("%d. op: %d, reg %d: %d, reg %d: %d, reg %d: %d\n", pos, operator, A, regs[A], B, regs[B], C, regs[C]);

        switch (operator) 
        {
            case 0:
                if (regs[C] != 0) regs[A] = regs[B];
                break;
            case 1:
                if (regs[B] != 0) arr = (struct array *)regs[B]; else arr = zero;
                regs[A] = arr->elements[regs[C]];
                break;
            case 2:
                if (regs[A] != 0) arr = (struct array *)regs[A]; else arr = zero;
                arr->elements[regs[B]] = regs[C];
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
            case 8: {
                arr = malloc(sizeof(*arr)+regs[C]*sizeof(uint));
                if (arr == NULL) die("new array memory allocation failed");
                memset(arr, 0, sizeof(*arr)+regs[C]*sizeof(uint));
                arr->length = regs[C];
                regs[B] = (uint)arr;
                break; }
            case 9:
                free((struct array*)regs[C]);
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
                    arr = (struct array *)regs[B];
                    progLen = arr->length;
                    zero = malloc(sizeof(*arr)+progLen*sizeof(uint));
                    if (zero == NULL) die("program array memory allocation failed");
                    memcpy(zero, arr, sizeof(*arr)+progLen*sizeof(uint)); }
                pos = regs[C]-1; // pos will ++ at the next loop
                break;
            case 13:
                regs[(instruction >> 25) & 7] = instruction & 0x01FFFFFF;
                break;
            default:
                die("unknown instruction; machine halted");
        };
    };
    return 0;
};