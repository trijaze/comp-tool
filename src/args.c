#include "../include/args.h"
#include "../include/logger.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h> 

int parse_args(int argc, char *argv[], Arguments *args) {
    int opt;
    args->verbose = 0;
    memset(args->password, 0, MAX_PASSWORD_LEN);

    // Xử lý tùy chọn -v
    while ((opt = getopt(argc, argv, "v")) != -1) {
        switch (opt) {
            case 'v':
                args->verbose = 1;
                break;
            default:
                return 0;
        }
    }

    // Còn lại: operation, format, input, output (4 tham số)
    if (optind + 3 > argc) {
        log_error("Expected at least 4 arguments: operation format input output\n");
        return 0;
    }

    strncpy(args->operation, argv[optind], sizeof(args->operation));
    strncpy(args->format, argv[optind + 1], sizeof(args->format));
    strncpy(args->input, argv[optind + 2], sizeof(args->input));
    //strncpy(args->output, argv[optind + 3], sizeof(args->output));

    int next_arg = optind + 4;

    // Kiểm tra nếu có --password option
    while (next_arg < argc) {
        if (strcmp(argv[next_arg], "--password") == 0 && next_arg + 1 < argc) {
            strncpy(args->password, argv[next_arg + 1], MAX_PASSWORD_LEN);
            next_arg += 2;
        } else {
            log_error("Unknown argument: %s\n", argv[next_arg]);
            return 0;
        }

    }

    // Kiểm tra operation hợp lệ
    if (strcmp(args->operation, "compress") != 0 &&
        strcmp(args->operation, "decompress") != 0 &&
        strcmp(args->operation, "pack") != 0 && 
        strcmp(args->operation, "unpack") != 0 &&
        strcmp(args->operation, "encrypt") != 0 &&
        strcmp(args->operation, "decrypt") != 0) {
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

    // Kiểm tra file input tồn tại (trừ các lệnh đọc)
    if (access(args->input, F_OK) != 0 &&
        strcmp(args->operation, "decompress") != 0 &&
        strcmp(args->operation, "unpack") != 0 &&
        strcmp(args->operation, "decrypt") != 0) {
        log_error("Input file %s does not exist\n", args->input);
        return 0;
    }

    if (optind + 3 < argc) {
        strncpy(args->output, argv[optind + 3], sizeof(args->output));
    } else {
    // Tạo tên folder từ tên file input
        char *base = basename(args->input);
        char *dot = strrchr(base, '.');
        if (dot) *dot = '\0';  // Xóa phần mở rộng như .zip, .enc...
        snprintf(args->output, sizeof(args->output), "%s", base);
    }


    return 1;
}

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [-v] <operation> <format> <input> <output> [--password <pwd>]\n", prog_name);
    fprintf(stderr, "Operations: compress, decompress, pack, unpack, encrypt, decrypt\n");
    fprintf(stderr, "Formats: zip, huffman, rle, tar\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -v              Verbose output\n");
    fprintf(stderr, "  --password pwd  Specify password\n");
}
