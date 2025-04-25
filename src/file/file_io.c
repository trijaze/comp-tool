// src/file/file_io.c
#include "../../include/file_io.h"
#include "../../include/tar_format.h"
#include "../../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

size_t read_file(const char *path, uint8_t **buffer) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        log_error("Cannot open file %s\n", path);
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);
    *buffer = (uint8_t *)malloc(size);
    if (!*buffer) {
        log_error("Memory allocation failed\n");
        fclose(fp);
        return 0;
    }
    if (fread(*buffer, 1, size, fp) != size) {
        log_error("Failed to read file %s\n", path);
        free(*buffer);
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return size;
}

int write_file(const char *path, const uint8_t *data, size_t size) {
    FILE *fp = fopen(path, "wb");
    if (!fp) {
        log_error("Cannot open file %s\n", path);
        return 1;
    }
    if (fwrite(data, 1, size, fp) != size) {
        log_error("Failed to write file %s\n", path);
        fclose(fp);
        return 1;
    }
    fclose(fp);
    return 0;
}

int write_file_to_tar(FILE *out, const char *filepath) {
    struct stat st;
    if (stat(filepath, &st) != 0) {
        log_error("Cannot stat file %s\n", filepath);
        return 1;
    }

    // Tạo tiêu đề TAR
    TarHeader header = {0};
    strncpy(header.name, filepath, sizeof(header.name) - 1);
    snprintf(header.mode, sizeof(header.mode), "%07o", st.st_mode & 0777);
    snprintf(header.uid, sizeof(header.uid), "%07o", st.st_uid);
    snprintf(header.gid, sizeof(header.gid), "%07o", st.st_gid);
    snprintf(header.size, sizeof(header.size), "%011lo", (long)st.st_size);
    snprintf(header.mtime, sizeof(header.mtime), "%011lo", (long)st.st_mtime);
    header.typeflag = '0'; // File thường
    strncpy(header.magic, "ustar", 5);
    header.version[0] = '0';
    strncpy(header.uname, "user", sizeof(header.uname) - 1);
    strncpy(header.gname, "group", sizeof(header.gname) - 1);

    // Tính checksum
    memset(header.checksum, ' ', sizeof(header.checksum));
    unsigned int sum = 0;
    for (size_t i = 0; i < sizeof(TarHeader); i++) {
        sum += ((unsigned char *)&header)[i];
    }
    snprintf(header.checksum, sizeof(header.checksum), "%06o", sum);

    // Ghi tiêu đề
    if (fwrite(&header, sizeof(TarHeader), 1, out) != 1) {
        log_error("Failed to write TAR header\n");
        return 1;
    }

    // Ghi nội dung file
    uint8_t *buffer;
    size_t size = read_file(filepath, &buffer);
    if (size == 0) {
        return 1;
    }
    if (fwrite(buffer, 1, size, out) != size) {
        log_error("Failed to write file content to TAR\n");
        free(buffer);
        return 1;
    }
    free(buffer);

    // Đệm khối 512 byte
    size_t padding = (TAR_BLOCK_SIZE - (size % TAR_BLOCK_SIZE)) % TAR_BLOCK_SIZE;
    char pad[TAR_BLOCK_SIZE] = {0};
    if (padding > 0 && fwrite(pad, 1, padding, out) != padding) {
        log_error("Failed to write TAR padding\n");
        return 1;
    }

    return 0;
}

int extract_tar_to_directory(FILE *in, const char *output_dir) {
    TarHeader header;
    while (fread(&header, sizeof(TarHeader), 1, in) == 1) {
        if (header.name[0] == '\0') break; // Kết thúc archive

        // Đọc kích thước file
        long size;
        sscanf(header.size, "%lo", &size);

        // Tạo đường dẫn đầu ra
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", output_dir, header.name);

        // Ghi file
        FILE *out = fopen(path, "wb");
        if (!out) {
            log_error("Cannot create file %s\n", path);
            return 1;
        }

        char buffer[4096];
        long remaining = size;
        while (remaining > 0) {
            size_t to_read = remaining < sizeof(buffer) ? remaining : sizeof(buffer);
            size_t read = fread(buffer, 1, to_read, in);
            if (read != to_read) {
                log_error("Failed to read file content from TAR\n");
                fclose(out);
                return 1;
            }
            if (fwrite(buffer, 1, read, out) != read) {
                log_error("Failed to write file %s\n", path);
                fclose(out);
                return 1;
            }
            remaining -= read;
        }
        fclose(out);

        // Bỏ qua padding
        size_t padding = (TAR_BLOCK_SIZE - (size % TAR_BLOCK_SIZE)) % TAR_BLOCK_SIZE;
        if (padding > 0 && fseek(in, padding, SEEK_CUR) != 0) {
            log_error("Failed to skip TAR padding\n");
            return 1;
        }
    }
    return 0;
}