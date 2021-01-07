#include "encryption/encryption.h"

void ENC_init(
    encryptor_t *enc,
    const char *public_key,
    unsigned long exp
)
{
    enc->keypair = RSA_new();
    enc->n = BN_new();
    enc->e = BN_new();
    enc->d = BN_new();

    BN_set_word(enc->e, exp);
    BN_hex2bn(&enc->n, public_key);
    RSA_set0_key(enc->keypair, enc->n, enc->e, enc->d);
}

void ENC_clear(
    encryptor_t *enc
)
{
    BN_free(enc->e);
    BN_free(enc->d);
    BN_free(enc->n);
    RSA_free(enc->keypair);
}

int ENC_encrypt(
    encryptor_t *enc,
    void *data2enc,
    uint32_t data_size,
    void *buffer2place,
    uint32_t buffer_offset
)
{
    int len;

    if (
        (
            len = RSA_public_encrypt
            (
                data_size,
                (unsigned char *)data2enc,
                (unsigned char *)buffer2place + buffer_offset,
                enc->keypair,
                RSA_PKCS1_OAEP_PADDING
            )
        ) == -1
    )
    {
    #if ENC_DEBUG == 1
        char *error_buffer = (char *) malloc(130);

        ERR_load_crypto_strings();
        ERR_error_string(ERR_get_error(), error_buffer);
        fprintf(stderr, "Error encrypting message: %s\n", error_buffer);

        free(error_buffer);
        return len;
    #endif
    }

    return len;
}

void DEC_init(
    decryptor_t *dec,
    int key_size,
    unsigned long exp
)
{
    dec->keypair = RSA_generate_key(key_size, exp, NULL, NULL);
}

void DEC_clear(
    decryptor_t *dec
)
{
    RSA_free(dec->keypair);
}

int DEC_decrypt(
    decryptor_t *dec,
    void *data2dec,
    uint32_t data_size,
    void *buffer2place,
    uint32_t buffer_offset
)
{
    if (
        RSA_private_decrypt
        (
            data_size,
            (unsigned char *)data2dec,
            (unsigned char *)buffer2place + buffer_offset,
            dec->keypair,
            RSA_PKCS1_OAEP_PADDING
        ) == -1
    )
    {
    #if ENC_DEBUG == 1
        char *error_buffer = (char *) malloc(130);

        ERR_load_crypto_strings();
        ERR_error_string(ERR_get_error(), error_buffer);
        fprintf(stderr, "Error encrypting message: %s\n", error_buffer);

        free(error_buffer);
        return EXIT_FAILURE;
    #endif
    }
    return EXIT_SUCCESS;
}

const BIGNUM *DEC_get_public_key(
    decryptor_t *dec
)
{
    return RSA_get0_n((const RSA*)dec->keypair);
}
