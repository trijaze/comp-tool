// src/compress/rle.c
#include "../../include/compress.h"
#include "../../include/logger.h"
#include <stdio.h>

int rle_compress(const char *input, const char *output) {
    FILE *in = fopen(input, "rb");
    if (!in) {
        log_error("Cannot open input file %s\n", input);
        return 1;
    }
    FILE *out = fopen(output, "wb");
    if (!out) {
        log_error("Cannot open output file %s\n", output);
        fclose(in);
        return 1;
    }

    int prev = fgetc(in), count = 1, curr;
    if (prev == EOF) {
        log_info("Input file %s is empty\n", input);
        fclose(in);
        fclose(out);
        return 0;
    }

    while ((curr = fgetc(in)) != EOF) {
        if (curr == prev && count < 255) {
            count++;
        } else {
            fputc(count, out);
            fputc(prev, out);
            count = 1;
            prev = curr;
        }
    }
    fputc(count, out);
    fputc(prev, out);

    fclose(in);
    fclose(out);
    return 0;
}

int rle_decompress(const char *input, const char *output) {
    FILE *in = fopen(input, "rb");
    if (!in) {
        log_error("Cannot open input file %s\n", input);
        return 1;
    }
    FILE *out = fopen(output, "wb");
    if (!out) {
        log_error("Cannot open output file %s\n", output);
        fclose(in);
        return 1;
    }

    int count, ch;
    while ((count = fgetc(in)) != EOF && (ch = fgetc(in)) != EOF) {
        for (int i = 0; i < count; i++) {
            fputc(ch, out);
        }
    }

    fclose(in);
    fclose(out);
    return 0;
}