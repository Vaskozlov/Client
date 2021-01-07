#ifndef enctyption_h
#define enctyption_h

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <inttypes.h>
#include <stdlib.h>

#define ENC_DEBUG 1

typedef struct package_data
{
    void *buffer;
    uint32_t size;
} __attribute__((packed)) package_data_t;

typedef struct encryptor
{
    RSA *keypair;
    BIGNUM *n, *e, *d;
} __attribute__((packed)) encryptor_t;

typedef struct decryptor
{
    RSA *keypair;
} __attribute__((packed)) decryptor_t;

#if defined __cplusplus
    extern "C" {
#endif /* __cplusplus */

void ENC_clear
(
    encryptor_t *enc
);

void ENC_init(
    encryptor_t *enc,
    const char *public_key,
    unsigned long exp
);

int ENC_encrypt(
    encryptor_t *enc,
    void *data2enc,
    uint32_t data_size,
    void *buffer2place,
    uint32_t buffer_offset
);

void DEC_clear
(
    decryptor_t *dec
);

void DEC_init(
    decryptor_t *dec,
    int key_size,
    unsigned long exp
);

int DEC_decrypt(
    decryptor_t *dec,
    void *data2dec,
    uint32_t data_size,
    void *buffer2place,
    uint32_t buffer_offset
);

const BIGNUM *DEC_get_public_key(
    decryptor_t *dec
);

#if defined __cplusplus
    }
#endif /* __cplusplus */

#endif /* encryption */