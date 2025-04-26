// src/tar/tar_unpack.c
#include "../../include/tar_format.h"
#include "../../include/file_io.h"
#include "../../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int tar_unpack(const char *tar_file, const char *output_dir) {
    log_info("Unpacking tar archive %s into %s\n", tar_file, output_dir);

    // Kiểm tra xem thư mục output có tồn tại không, nếu không tạo mới
    struct stat st;
    if (stat(output_dir, &st) != 0) {
        if (mkdir(output_dir, 0755) != 0) {
            log_error("Cannot create directory %s\n", output_dir);
            return 0;
        }
    }

    FILE *in = fopen(tar_file, "rb");
    if (!in) {
        log_error("Cannot open tar archive %s\n", tar_file);
        return 0;
    }

    if (extract_tar_to_directory(in, output_dir) != 0) {
        fclose(in);
        return 0;
    }

    fclose(in);
    return 1; 
}
