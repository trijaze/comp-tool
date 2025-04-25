// src/tar/tar_unpack.c
#include "../../include/tar_format.h"
#include "../../include/file_io.h"
#include "../../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void tar_unpack(const char *tar_file, const char *output_dir) {
    log_info("Unpacking tar archive %s into %s\n", tar_file, output_dir);

    // Tạo thư mục đầu ra nếu chưa tồn tại
    struct stat st;
    if (stat(output_dir, &st) != 0) {
        if (mkdir(output_dir, 0755) != 0) {
            log_error("Cannot create directory %s\n", output_dir);
            return;
        }
    }

    FILE *in = fopen(tar_file, "rb");
    if (!in) {
        log_error("Cannot open tar archive %s\n", tar_file);
        return;
    }

    if (extract_tar_to_directory(in, output_dir) != 0) {
        fclose(in);
        return;
    }

    fclose(in);
}