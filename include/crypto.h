#ifndef CRYPTO_H
#define CRYPTO_H

int encrypt_file(const char *input_path, const char *output_path, const char *key);
int decrypt_file(const char *input_path, const char *output_path, const char *key);

#endif
