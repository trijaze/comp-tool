// src/zip/zip_decompress.c
#include "../../include/compress.h"
#include "../../include/file_io.h"
#include "../../include/zip_format.h"
#include "../../include/crc32.h"
#include "../../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int zip_decompress(const char *zip_file, const char *output_path) {
    init_crc32();

    FILE *in = fopen(zip_file, "rb");
    if (!in) {
        log_error("Cannot open zip file %s\n", zip_file);
        return 1;
    }

    FILE *out = fopen(output_path, "wb");
    if (!out) {
        log_error("Cannot open output file %s\n", output_path);
        fclose(in);
        return 1;
    }

    // Đọc tiêu đề
    ZipLocalFileHeader header;
    if (fread(&header, sizeof(ZipLocalFileHeader), 1, in) != 1) {
        log_error("Invalid ZIP header\n");
        fclose(in);
        fclose(out);
        return 1;
    }
    if (header.signature != ZIP_LOCAL_FILE_SIGNATURE) {
        log_error("Not a ZIP file\n");
        fclose(in);
        fclose(out);
        return 1;
    }

    // Bỏ qua tên file và extra field
    fseek(in, header.filename_len + header.extra_len, SEEK_CUR);

    // Đọc dữ liệu
    uint8_t *data = (uint8_t *)malloc(header.compressed_size);
    if (!data) {
        log_error("Memory allocation failed\n");
        fclose(in);
        fclose(out);
        return 1;
    }
    if (fread(data, 1, header.compressed_size, in) != header.compressed_size) {
        log_error("Failed to read ZIP data\n");
        free(data);
        fclose(in);
        fclose(out);
        return 1;
    }

    // Kiểm tra CRC32
    uint32_t crc = compute_crc32(data, header.compressed_size);
    if (crc != header.crc32) {
        log_error("CRC32 mismatch\n");
        free(data);
        fclose(in);
        fclose(out);
        return 1;
    }

    // Ghi dữ liệu
    if (fwrite(data, 1, header.compressed_size, out) != header.compressed_size) {
        log_error("Failed to write output file %s\n", output_path);
        free(data);
        fclose(in);
        fclose(out);
        return 1;
    }

    free(data);
    fclose(in);
    fclose(out);
    return 0;
}