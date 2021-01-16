#include "gclient.hpp"

template <typename T>
static T make_alignedl(T number, int power)
{
    return number + ((1 << power) - (number & ((1 << power) - 1))); 
}

auto gclient::change_dir(const char* path) -> int
{
    char *buffer = (char *) alloca(512);
    strcpy(buffer, path);

    send_status(CHANGE_DIR_CODE);
    send_encrypted(buffer, strlen(buffer));
    return recv_status();
}

auto gclient::proccess_command(const char *command) -> package_data_t
{
    send_status(2);
    send_encrypted((void *)command, strlen(command));
    return recv_encrypyted();
}

auto gclient::download(const char *fillename, const char *outputname) -> int
{
    char path[512], path2[512];
    send_status(3);
    send_encrypted((void *)fillename, strlen(fillename));

    auto data = download_packages();
    int fd = open(".output.zip", O_RDWR | O_CREAT | O_TRUNC, 0666);
    
    if (fd > 0)
        write(fd, data.buffer, data.size);

    free(data.buffer);
    close(fd);
    
    getcwd(path, 300);

    strcpy(path2, path);
    strcat(path2, "/");
    strcat(path2, ".output.zip");

    unzip(path2, path);
    remove(path2);

    return !(fd > 0);
}

auto gclient::send_file(const char *fillename, const char *outputfile) -> int
{
    char *path = (char*) alloca(256);
    char *path2 = (char*) alloca(512);

    strcpy(path, getenv("HOME"));
    strcat(path, "/.gserv_cache");
    
    create_dir(path);
    strcat(path, "/out.zip");

    getcwd(path2, 256);
    strcat(path2, "/");
    strcat(path2, fillename);

    zip(path2, path);

    auto fileout = fopen(path, "r");

	fseek(fileout, 0L, SEEK_END);
	long size = ftell(fileout);
    unsigned i = 0;
    
    char *buffer = (char*) malloc(
        make_alignedl(size, UNENCRYPTED_PACKAGE_POWER)
    );
	fseek(fileout, 0L, SEEK_SET);

	for (int ch = fgetc(fileout); ch != EOF; ch = fgetc(fileout))
		buffer[i++] = ch;

	fclose(fileout);
    remove(path);

    send_status(4);
    send_package((void*)outputfile, strlen(outputfile) + 1);
    send_package(buffer, size);

    return recv_status();
}

gclient::gclient(
    const char *ip_addr,
    uint16_t port_addr,
    const char *username,
    const char *password,
    const char *root_password 
)
:   root(false),
    logined(false),
    cli::client::client(ip_addr, port_addr)
{

    if (connect() != EXIT_SUCCESS)
    {
        fputs("Unable to connect to servre", stderr);
    }

    if (username != NULL && password != NULL)
    {
        if (login(username, password) == EXIT_SUCCESS)
            logined = true;
    }

    if (root_password != NULL && logined)
    {
        if (get_root_access(root_password) == EXIT_SUCCESS)
            root = true;
    }

}