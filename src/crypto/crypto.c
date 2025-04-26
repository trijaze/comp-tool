#include "../../include/crypto.h"
#include <stdio.h>
#include <string.h>  // <== Cần thiết để dùng strlen

int encrypt_file(const char *input_path, const char *output_path, const char *key) {
    FILE *in = fopen(input_path, "rb");
    FILE *out = fopen(output_path, "wb");
    if (!in || !out) return 1;

    int keylen = strlen(key);
    int i = 0;
    int ch;
    while ((ch = fgetc(in)) != EOF) {
        fputc(ch ^ key[i % keylen], out);
        i++;
    }

    fclose(in);
    fclose(out);
    return 0;
}

int decrypt_file(const char *input_path, const char *output_path, const char *key) {
    // XOR decryption is symmetric
    return encrypt_file(input_path, output_path, key);
}
