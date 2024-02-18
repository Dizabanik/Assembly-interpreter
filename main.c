#include <stdio.h>
#include "asm.h"
#include <stdlib.h>

int main(int argc, char **argv){
    FILE *f;
    f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    char* s = assembler_interpreter(string);
    if(s == (char*)-1) return -1;
    printf("%s\n", s);
    return 0;
}