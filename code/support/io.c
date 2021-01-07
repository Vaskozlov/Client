#include "support/io.h"

char *read_file(const char *filename)
{
    long size;
    char *buffer;
    unsigned long i = 0UL;
    FILE *file = fopen(filename, "r");

    fseek(file, 0L, SEEK_END);
    size = ftell(file) + 1;
    buffer = (char *)malloc(size);
    
    for (int ch = fgetc(file); ch != EOF; ch = fgetc(file))
        buffer[i++] = (char) ch;

    buffer[i] = '\0';
    
    fclose(file);
    return buffer;
}
