// src/tar/tar_pack.c
#include "../../include/tar_format.h"
#include "../../include/file_io.h"
#include "../../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tar_pack(const char *input_path, const char *tar_file) {
    log_info("Packing %s into tar archive %s\n", input_path, tar_file);
    FILE *out = fopen(tar_file, "wb");
    if (!out) {
        log_error("Cannot create tar file %s\n", tar_file);
        return;
    }

    if (write_file_to_tar(out, input_path) != 0) {
        fclose(out);
        return;
    }

    // Ghi khối kết thúc (2 khối 0)
    char end_block[TAR_BLOCK_SIZE] = {0};
    fwrite(end_block, TAR_BLOCK_SIZE, 2, out);
    fclose(out);
}