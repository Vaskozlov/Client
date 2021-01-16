#ifndef archive_h
#define archive_h

#include <zip.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "support/io.h"

#if defined __cplusplus
    extern "C" {
#endif /* __cplusplus */

int create_dir(
    const char *dirname
);

int is_dir(
    const char *path
);

int exits(
    const char *path
);

int zip(
    const char *path,
    const char *output_filename
);

int unzip(
    const char *path2archive,
    const char *path2extract
);

#if defined __cplusplus
    }
#endif /* __cplusplus */

#endif /* encryption */
