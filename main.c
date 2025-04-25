#include "include/args.h"
#include "include/logger.h"
#include "include/compress.h"
#include "include/file_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    Arguments args = {0};
    init_logger("comp-tool.log"); // Khởi tạo logger

    if (!parse_args(argc, argv, &args)) {
        print_usage(argv[0]);
        close_logger();
        return 1;
    }

    if (args.verbose) {
        log_info("Operation: %s\n", args.operation);
        log_info("Format: %s\n", args.format);
        log_info("Input: %s\n", args.input);
        log_info("Output: %s\n", args.output);
    }

    int result = 0;
    if (strcmp(args.operation, "compress") == 0) {
        if (strcmp(args.format, "zip") == 0) {
            result = zip_compress(args.input, args.output);
        } else if (strcmp(args.format, "huffman") == 0) {
            result = huffman_compress(args.input, args.output);
        } else if (strcmp(args.format, "rle") == 0) {
            result = rle_compress(args.input, args.output);
        } else {
            log_error("Unsupported format: %s\n", args.format);
            result = 1;
        }
    } else if (strcmp(args.operation, "decompress") == 0) {
        if (strcmp(args.format, "zip") == 0) {
            result = zip_decompress(args.input, args.output);
        } else if (strcmp(args.format, "huffman") == 0) {
            result = huffman_decompress(args.input, args.output);
        } else if (strcmp(args.format, "rle") == 0) {
            result = rle_decompress(args.input, args.output);
        } else {
            log_error("Unsupported format: %s\n", args.format);
            result = 1;
        }
    } else if (strcmp(args.operation, "pack") == 0 && strcmp(args.format, "tar") == 0) {
        tar_pack(args.input, args.output);
    } else if (strcmp(args.operation, "unpack") == 0 && strcmp(args.format, "tar") == 0) {
        tar_unpack(args.input, args.output);
    } else {
        log_error("Unknown operation or format: %s, %s\n", args.operation, args.format);
        result = 1;
    }

    close_logger();
    return result;
}