#include "client/client.hpp"

namespace cli
{

    int client::connect()
    {
        uint16_t status;
        DEC_init(&dec, KEY_SIZE, ENC_EXP);

        if (
            (
                socket = ::socket(AF_INET, SOCK_STREAM, 0)
            ) <= 0
        )
        {
            return BAD_SOCKET;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        
        if (
            inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0
        )
        {
            return INVALID_ADDRESS;
        }

        if (
            ::connect(
                socket,
                (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)
            ) < 0
        )
        {
            return SERVER_ERROR;
        }

        if (::recv(socket, &status, sizeof(uint16_t), 0) <= 0)
        {
            return SERVER_ERROR;
        }

        if (status != EXIT_SUCCESS)
        {
            return SERVER_FULL;
        }
        const BIGNUM *key = DEC_get_public_key(&dec);
        char *public_key = BN_bn2hex(key);

        recv_buffer(buffer, KEY_SIZE >> 2);
        buffer[KEY_SIZE >> 2] = '\0';

        send_buffer(public_key, KEY_SIZE >> 2);
        ENC_init(&enc, public_key, ENC_EXP);

        free(public_key);

        connected = true;
        return EXIT_SUCCESS;
    }

    client::client(const char *ip_addr, uint16_t port_addr)
    :   port(port_addr)
    {
        strcpy(ip, ip_addr);
    }

    client::~client()
    {
        if (connected)
        {
            send_status(0xFFFF);
        }
    }

}