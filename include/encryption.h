#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stddef.h>
#include <stdint.h>

void xor_encrypt(uint8_t *data, size_t len, const char *key);
void xor_decrypt(uint8_t *data, size_t len, const char *key);

#endif
