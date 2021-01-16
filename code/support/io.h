#ifndef io_h
#define io_h

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef struct define_struct
{
    uint8_t is_num;
    char *name;
    char *placeholder;
} define_struct_t;

#if defined __cplusplus
    extern "C" {
#endif /* __cplusplus */

unsigned long str2bn(const char *str);
char *read_file(const char *filename);
char **load_config(
    char *filename,
    char **fields, 
    int amount_of_fields
);

#if defined __cplusplus
    }
#endif /* __cplusplus */

#endif /* io_h */
