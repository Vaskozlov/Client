#include "support/archive.h"

static int create_dir(const char *dirname)
{
    if (mkdir(dirname, 0755) < 0)
    {
        if (errno != EEXIST)
        {
            perror(dirname);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

static const int is_dir(const char *path)
{
    struct stat st;
    stat(path, &st);
    return S_ISDIR(st.st_mode);
}

int walker(
    int startdir_len,
    const char *path,
    zip_t *zipper
)
{
    int len = strlen(path);
    DIR *directory = opendir(path);
    char *fullpath = (char *)malloc(512);
    struct dirent *dirp;

    if (directory == NULL)
    {
        free(fullpath);
        return EXIT_FAILURE;
    }

    strcpy(fullpath, path);

    while ((dirp = readdir(directory)) != NULL)
    {
        if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
        {
            fullpath[len] = '/';
            fullpath[len + 1] = '\0';
            strcat(fullpath, dirp->d_name);

            if (is_dir(fullpath))
            {
                if (zip_dir_add(zipper, fullpath + startdir_len, ZIP_FL_ENC_UTF_8) < 0)
                {
                    free(fullpath);
                    return EXIT_FAILURE;
                }
                if (walker(startdir_len, fullpath, zipper) != EXIT_SUCCESS)
                {
                    free(fullpath);
                    return EXIT_FAILURE;
                }
            }
            else
            {
                zip_source_t *source = zip_source_file(zipper, fullpath, 0, 0);

                if (source == NULL)
                {
                    free(fullpath);
                    return EXIT_FAILURE;
                }

                if (zip_file_add(zipper, fullpath + startdir_len, source, ZIP_FL_ENC_UTF_8) < 0)
                {
                    zip_source_free(source);
                    free(fullpath);
                    return EXIT_FAILURE;
                }
            }
        }
    }
    closedir(directory);
    free(fullpath);
    return EXIT_SUCCESS;
}

int zip(
    const char *path,
    const char *output_filename
)
{
    int error;

    zip_t *zipper = zip_open(output_filename, ZIP_CREATE | ZIP_EXCL, &error);

    if (zipper == NULL || error == 0)
    {
        zip_error_t ziperror;
        zip_error_init_with_code(&ziperror, error);
        printf(stderr, "Failed to open output file %s : %s", output_filename, zip_error_strerror(&ziperror));
        return EXIT_FAILURE;
    }

    if (is_dir(path))
    {
        walker(strlen(path) + 1, path, zipper);
    }
    else
    {
        zip_source_t *source = zip_source_file(zipper, path, 0, 0);

        if (source == NULL)
            return EXIT_FAILURE;
        if (zip_file_add(zipper, path, source, ZIP_FL_ENC_UTF_8) < 0)
            return EXIT_FAILURE;
    }
    
    zip_close(zipper);
    return EXIT_SUCCESS;
}

int unzip(
    const char *path2archive,
    const char *path2extract
)
{
    int fd;
    int len;
    int error;
    int exlen;
    long long sum;
    zip_t *zipper;
    zip_stat_t sb;
    struct zip_file *file;
    char buffer[100];
    char *path_buffer = (char*) malloc(512);

    strcpy(path_buffer, path2extract);
    exlen = strlen(path2extract);

    if (
        (
            zipper = zip_open(path2archive, 0, &error)
        ) == NULL
    )
    {
        zip_error_t ziperror;
        zip_error_init_with_code(&ziperror, error);
        printf(stderr, "Failed to open output file %s : %s", path2archive, zip_error_strerror(&ziperror));
        return EXIT_FAILURE;
    }

    for (int i = 0; i < zip_get_num_entries(zipper, 0); i++)
    {
        if (zip_stat_index(zipper, i, 0, &sb) == 0)
        {
            
            len = strlen(sb.name);
            path_buffer[exlen] = '/';
            path_buffer[exlen + 1] = '\0';

            strcat(path_buffer, sb.name);

            if (sb.name[len -1] == '/')
                create_dir(path_buffer);
            else
            {
                file = zip_fopen_index(zipper, i, 0);

                if (file == NULL)
                {
                    free(path_buffer);
                    return EXIT_FAILURE;
                }

                fd = open(path_buffer, O_RDWR | O_TRUNC | O_CREAT, 0644);

                if (fd < 0)
                {
                    free(path_buffer);
                    return EXIT_FAILURE;
                }

                sum = 0;

                while (sum != sb.size)
                {
                    len = zip_fread(file, buffer, 100);

                    if (len < 0)
                    {
                        free(path_buffer);
                        return EXIT_FAILURE;
                    }

                    write(fd, buffer, len);
                    sum += len;
                }
                close(fd);
                zip_fclose(file);
            }
        }
    }
    zip_close(zipper);
    free(path_buffer);
}