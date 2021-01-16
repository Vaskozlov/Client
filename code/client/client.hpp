#ifndef client_hpp
#define client_hpp

#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <stdarg.h>

#include "support/archive.h"
#include "encryption/encryption.h"

static constexpr uint32_t calculate_power(uint32_t number)
{
    for (uint32_t i = 0; i < sizeof(uint32_t) * 8; i++)
    {
        if ((number >> i) == 1)
            return i;
    }
    return -1;
}

#define ENC_EXP 3

#define BAD_SOCKET -1
#define MAX_CONNECTIONS -2
#define SERVER_FULL -3
#define INVALID_ADDRESS -4
#define SERVER_ERROR -5

#define KEY_SIZE 4096
#define KEY_POWER calculate_power(KEY_SIZE)

#define ENCRYPTED_PACKAGE_SIZE (KEY_SIZE / 8)
#define ENCRYPTED_PACKAGE_BITS (ENCRYPTED_PACKAGE_SIZE - 1)
#define ENCRYPTED_PACKAGE_POWER calculate_power(ENCRYPTED_PACKAGE_SIZE)

#define UNENCRYPTED_PACKAGE_SIZE (ENCRYPTED_PACKAGE_SIZE / 2)
#define UNENCRYPTED_PACKAGE_BITS (UNENCRYPTED_PACKAGE_SIZE - 1)
#define UNENCRYPTED_PACKAGE_POWER calculate_power(UNENCRYPTED_PACKAGE_SIZE)

namespace cli
{

struct client
{
    bool connected;
    uint16_t port;

    int socket;
    int maximum_calls;

    decryptor_t dec;
    encryptor_t enc;

    struct sockaddr_in serv_addr;
    char ip[32];
    char buffer[1025];

public:
    inline void send_status(uint16_t status)
    {
        if (::send(socket, &status, sizeof(uint16_t), 0) <= 0)
        {
            throw std::runtime_error("Unable to send status");
        }
    }

    inline uint16_t recv_status()
    {
        uint16_t status;

        if (::recv(socket, &status, sizeof(uint16_t), 0) <= 0)
        {
            throw std::runtime_error("Unable to recv status");
        }

        return status;
    }

    inline void send_buffer(void *buffer2send, uint32_t buffer_size)
    {
        if (::send(socket, buffer2send, buffer_size, 0) <= 0)
        {
            throw std::runtime_error("Unable to send status");
        }
    }

    inline void recv_buffer(void *buffer2place, uint32_t buffer_size)
    {
        if (::recv(socket, buffer2place, buffer_size, 0) <= 0)
        {
            throw std::runtime_error("Unable to recv buffer");
        }
    }

public:
    package_data_t recv_encrypyted();
    package_data_t download_packages();
    void send_package(void* buffer, uint32_t size);
    int send_encrypted(void *buffer2send, uint64_t buffer_size);

public:
    int connect();
    int login(const char *username, const char *password);
    int get_root_access(const char *password);

public:
    ~client();
    client(const char *ip_addr, uint16_t port_addr);
};

}

#endif /* CLIENT_HPP */