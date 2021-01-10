#ifndef io_h
#define io_h

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined __cplusplus
    extern "C" {
#endif /* __cplusplus */

char *read_file(const char *filename);

#if defined __cplusplus
    }
#endif /* __cplusplus */

#endif /* io_h */
