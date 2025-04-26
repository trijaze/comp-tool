#include "../../include/compress.h"
#include "../../include/file_io.h"
#include "../../include/zip_format.h"
#include "../../include/crc32.h"
#include "../../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>

int zip_decompress(const char *zip_file, const char *output_path) {
    init_crc32();

    FILE *in = fopen(zip_file, "rb");
    if (!in) {
        log_error("Cannot open zip file %s\n", zip_file);
        return 1;
    }

    // Tạo thư mục đích nếu không tồn tại
    struct stat st = {0};
    if (stat(output_path, &st) == -1) {
        if (mkdir(output_path, 0777) == -1) {
            log_error("Failed to create directory %s\n", output_path);
            fclose(in);
            return 1;
        }
    }

    // Đọc tiêu đề ZIP
    ZipLocalFileHeader header;
    if (fread(&header, sizeof(ZipLocalFileHeader), 1, in) != 1) {
        log_error("Invalid ZIP header\n");
        fclose(in);
        return 1;
    }
    if (header.signature != ZIP_LOCAL_FILE_SIGNATURE) {
        log_error("Not a ZIP file\n");
        fclose(in);
        return 1;
    }

    // Kiểm tra độ dài tên file hợp lệ
    if (header.filename_len == 0 || header.filename_len > 1024) {
        log_error("Invalid filename length\n");
        fclose(in);
        return 1;
    }

    // Đọc tên file
    char *filename = malloc(header.filename_len + 1);
    if (!filename) {
        log_error("Memory allocation failed for filename\n");
        fclose(in);
        return 1;
    }
    if (fread(filename, 1, header.filename_len, in) != header.filename_len) {
        log_error("Failed to read filename from ZIP file\n");
        free(filename);
        fclose(in);
        return 1;
    }
    filename[header.filename_len] = '\0';

    // Bỏ qua extra field
    fseek(in, header.extra_len, SEEK_CUR);

    // Kiểm tra kích thước hợp lệ
    if (header.compressed_size == 0 || header.compressed_size > 50 * 1024 * 1024) {
        log_error("Compressed size too large or invalid\n");
        free(filename);
        fclose(in);
        return 1;
    }

    // Đọc dữ liệu nén
    uint8_t *data = (uint8_t *)malloc(header.compressed_size);
    if (!data) {
        log_error("Memory allocation failed\n");
        free(filename);
        fclose(in);
        return 1;
    }
    if (fread(data, 1, header.compressed_size, in) != header.compressed_size) {
        log_error("Failed to read ZIP data\n");
        free(data);
        free(filename);
        fclose(in);
        return 1;
    }

    // Kiểm tra CRC32
    uint32_t crc = compute_crc32(data, header.compressed_size);
    if (crc != header.crc32) {
        log_error("CRC32 mismatch\n");
        free(data);
        free(filename);
        fclose(in);
        return 1;
    }

    // Tạo đường dẫn file đầu ra
    char output_file[512];
    snprintf(output_file, sizeof(output_file), "%s/%s", output_path, filename);

    // Tạo thư mục cha nếu cần
    char *output_dup = strdup(output_file);
    char *dir_path = dirname(output_dup);
    struct stat st2 = {0};
    if (stat(dir_path, &st2) == -1) {
        if (mkdir(dir_path, 0777) == -1 && errno != EEXIST) {
            log_error("Failed to create subdirectory %s\n", dir_path);
            free(output_dup);
            free(data);
            free(filename);
            fclose(in);
            return 1;
        }
    }
    free(output_dup);

    // Ghi file ra đĩa
    FILE *out = fopen(output_file, "wb");
    if (!out) {
        log_error("Cannot open output file %s\n", output_file);
        free(data);
        free(filename);
        fclose(in);
        return 1;
    }

    if (fwrite(data, 1, header.compressed_size, out) != header.compressed_size) {
        log_error("Failed to write output file %s\n", output_file);
        free(data);
        free(filename);
        fclose(in);
        fclose(out);
        return 1;
    }

    free(data);
    free(filename);
    fclose(in);
    fclose(out);
    return 0;
}
