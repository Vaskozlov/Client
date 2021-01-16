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
        ENC_init(&enc, buffer, ENC_EXP);

        free(public_key);

        connected = true;
        return EXIT_SUCCESS;
    }

    int client::login(const char *username, const char *password)
    {
        char *local_buffer = (char*) alloca(256);
        const size_t len = strlen(username);
    
        strcpy(local_buffer, username);
        local_buffer[len] = '\n';
        local_buffer[len + 1] = '\0';
        strcat(local_buffer, password);

        send_status(0xFFFE);
        send_encrypted(local_buffer, 256);
        return recv_status();
    }

    int client::get_root_access(const char *password)
    {
        char *local_buffer = (char*) alloca(256);
        send_status(0xFFFD);

        strcpy(local_buffer, password);
        send_encrypted(local_buffer, 256);
        return recv_status();
    }

    void client::send_package(void* buffer, uint32_t size){
        uint32_t times, extra;
		int status;
        times = size >> 10;
        extra = size & 1023;
        
        send_buffer(&size, sizeof(uint32_t));
        
        for (uint32_t i = 0; i < times; i++){
            send_buffer((char*)buffer + (i << 10), 1024);
		}
        
        if (extra > 0){
            send_buffer((char*)buffer + size - extra, extra);
		}
    }

    int client::send_encrypted(void *buffer2send, uint64_t buffer_size)
    {
        uint64_t counter = 0;
        char *local_buffer = buffer;
        uint64_t packages = 
            (buffer_size >> UNENCRYPTED_PACKAGE_POWER) + 
            ((buffer_size & UNENCRYPTED_PACKAGE_BITS) > 0);
        uint64_t times = packages >> 1;
        send_buffer(&buffer_size, sizeof(uint64_t));

        for (uint64_t i = 0; i < times; i++)
        {
            ENC_encrypt(
                &enc,
                (char*)buffer + (counter++ << UNENCRYPTED_PACKAGE_POWER),
                UNENCRYPTED_PACKAGE_SIZE,
                local_buffer,
                0
            );
            ENC_encrypt(
                &enc,
                (char*)buffer + (counter++ << UNENCRYPTED_PACKAGE_POWER),
                UNENCRYPTED_PACKAGE_SIZE,
                local_buffer,
                ENCRYPTED_PACKAGE_SIZE 
            );

            send_buffer(
                local_buffer,
                ENCRYPTED_PACKAGE_SIZE << 1
            );
        }

        if ((packages & 0b1) > 0)
        {
            ENC_encrypt(
                &enc,
                (char*)buffer2send + (counter++ << UNENCRYPTED_PACKAGE_POWER),
                UNENCRYPTED_PACKAGE_SIZE,
                local_buffer,
                0
            );

            send_buffer(
                local_buffer,
                ENCRYPTED_PACKAGE_SIZE
            );
        }

        return EXIT_SUCCESS;
    }

    package_data_t client::download_packages(){
    	uint32_t package_size, times, extra;
        recv_buffer(&package_size, sizeof(uint32_t));
	    
	    times = package_size >> 10; // times /= 1024;
	    extra = (package_size & 1023);
	    char* local_buffer = (char*)malloc(package_size);
	    
	    for (uint32_t i = 0; i < times; i++)
	        recv_buffer(local_buffer + (i << 10), 1024);
	    
	    if (extra)
	        recv_buffer(local_buffer + (package_size - extra), extra);
	    
	    return {local_buffer, package_size};
	}

    package_data_t client::recv_encrypyted()
    {
        char *buffer2download;
        char *local_buffer = buffer;
        uint64_t size, packages, times, counter = 0;

        recv_buffer(&size, sizeof(uint64_t));

        packages = 
            (size >> UNENCRYPTED_PACKAGE_POWER) +
            ((size & UNENCRYPTED_PACKAGE_BITS) > 0);

        times = packages >> 1;
        buffer2download = (char *) malloc(packages << UNENCRYPTED_PACKAGE_POWER);

        for (uint64_t i = 0; i < times; i++)
        {
            recv_buffer(local_buffer, ENCRYPTED_PACKAGE_SIZE << 1);
      
            DEC_decrypt(
                &dec,
                local_buffer,
                ENCRYPTED_PACKAGE_SIZE,
                buffer2download + (counter++ << UNENCRYPTED_PACKAGE_POWER), 
                0
            );

            DEC_decrypt(
                &dec,
                local_buffer + ENCRYPTED_PACKAGE_SIZE,
                ENCRYPTED_PACKAGE_SIZE,
                buffer2download + (counter++ << UNENCRYPTED_PACKAGE_POWER),
                0
            );
        }

        if ((packages & 0b1) > 0)
        {
            recv_buffer(
                local_buffer,
                ENCRYPTED_PACKAGE_SIZE
            );

             DEC_decrypt(
                &dec,
                local_buffer,
                ENCRYPTED_PACKAGE_SIZE,
                buffer2download + (counter++ << UNENCRYPTED_PACKAGE_POWER), 
                0
            );
        }
        return {buffer2download, size};
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