#ifndef gclient_hpp
#define gclient_hpp

#include <iostream>
#include "client/client.hpp"

#define CHANGE_DIR_CODE 1

class gclient: public cli::client
{
    bool root;
    bool logined;

public:
    auto change_dir(const char* path) -> int;
    auto proccess_command(const char *command) -> package_data_t;
    auto download(const char *fillename, const char *outputname) -> int;
    auto send_file(const char *fillename, const char *outputfile) -> int;

public:
    gclient(
        const char *ip_addr,
        uint16_t port_addr,
        const char *username = NULL,
        const char *password = NULL,
        const char *root_password = NULL
    );
};

#endif /* gclient_hpp */