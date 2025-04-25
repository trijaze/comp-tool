// src/args.c
#include "../include/args.h"
#include "../include/logger.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int parse_args(int argc, char *argv[], Arguments *args) {
    int opt;
    args->verbose = 0;

    while ((opt = getopt(argc, argv, "v")) != -1) {
        switch (opt) {
            case 'v':
                args->verbose = 1;
                break;
            default:
                return 0;
        }
    }

    if (optind + 4 != argc) {
        log_error("Expected 4 arguments: operation format input output\n");
        return 0;
    }

    strncpy(args->operation, argv[optind], sizeof(args->operation));
    strncpy(args->format, argv[optind + 1], sizeof(args->format));
    strncpy(args->input, argv[optind + 2], sizeof(args->input));
    strncpy(args->output, argv[optind + 3], sizeof(args->output));

    // Kiểm tra operation hợp lệ
    if (strcmp(args->operation, "compress") != 0 &&
        strcmp(args->operation, "decompress") != 0 &&
        strcmp(args->operation, "pack") != 0 &&
        strcmp(args->operation, "unpack") != 0) {
        log_error("Invalid operation: %s\n", args->operation);
        return 0;
    }

    // Kiểm tra format hợp lệ
    if (strcmp(args->format, "zip") != 0 &&
        strcmp(args->format, "huffman") != 0 &&
        strcmp(args->format, "rle") != 0 &&
        strcmp(args->format, "tar") != 0) {
        log_error("Invalid format: %s\n", args->format);
        return 0;
    }

    // Kiểm tra file input tồn tại
    if (access(args->input, F_OK) != 0 && strcmp(args->operation, "decompress") != 0 &&
        strcmp(args->operation, "unpack") != 0) {
        log_error("Input file %s does not exist\n", args->input);
        return 0;
    }

    return 1;
}

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [-v] <operation> <format> <input> <output>\n", prog_name);
    fprintf(stderr, "Operations: compress, decompress, pack, unpack\n");
    fprintf(stderr, "Formats: zip, huffman, rle, tar\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -v  Verbose output\n");
}