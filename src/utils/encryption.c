#include "../include/encryption.h"
#include <string.h>

void xor_encrypt(uint8_t *data, size_t len, const char *key) {
    size_t keylen = strlen(key);
    for (size_t i = 0; i < len; ++i) {
        data[i] ^= key[i % keylen];
    }
}

void xor_decrypt(uint8_t *data, size_t len, const char *key) {
    xor_encrypt(data, len, key);  // XOR dùng cho cả encrypt/decrypt
}
