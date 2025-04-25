// src/zip/zip_compress.c
#include "../../include/compress.h"
#include "../../include/file_io.h"
#include "../../include/zip_format.h"
#include "../../include/crc32.h"
#include "../../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int zip_compress(const char *input_path, const char *output_file) {
    init_crc32(); // Khởi tạo bảng CRC32

    FILE *in = fopen(input_path, "rb");
    if (!in) {
        log_error("Cannot open input file %s\n", input_path);
        return 1;
    }

    FILE *out = fopen(output_file, "wb");
    if (!out) {
        log_error("Cannot open output file %s\n", output_file);
        fclose(in);
        return 1;
    }

    // Đọc dữ liệu file
    uint8_t *data;
    size_t size = read_file(input_path, &data);
    if (size == 0) {
        fclose(in);
        fclose(out);
        return 1;
    }

    // Tính CRC32
    uint32_t crc = compute_crc32(data, size);

    // Tạo tiêu đề ZIP
    ZipLocalFileHeader header = {0};
    header.signature = ZIP_LOCAL_FILE_SIGNATURE;
    header.version = 20; // Phiên bản 2.0
    header.compression = 0; // Không nén (stored)
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    header.mod_time = (tm->tm_hour << 11) | (tm->tm_min << 5) | (tm->tm_sec / 2);
    header.mod_date = ((tm->tm_year + 1900 - 1980) << 9) | ((tm->tm_mon + 1) << 5) | tm->tm_mday;
    header.crc32 = crc;
    header.compressed_size = size;
    header.uncompressed_size = size;
    header.filename_len = strlen(input_path);
    header.extra_len = 0;

    // Ghi tiêu đề
    if (fwrite(&header, sizeof(ZipLocalFileHeader), 1, out) != 1) {
        log_error("Failed to write ZIP header\n");
        free(data);
        fclose(in);
        fclose(out);
        return 1;
    }

    // Ghi tên file
    if (fwrite(input_path, 1, header.filename_len, out) != header.filename_len) {
        log_error("Failed to write filename\n");
        free(data);
        fclose(in);
        fclose(out);
        return 1;
    }

    // Ghi dữ liệu
    if (fwrite(data, 1, size, out) != size) {
        log_error("Failed to write data\n");
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