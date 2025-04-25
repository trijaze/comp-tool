// include/file_io.h
#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdint.h>
#include <stdio.h>

size_t read_file(const char *path, uint8_t **buffer);
int write_file(const char *path, const uint8_t *data, size_t size);
int write_file_to_tar(FILE *out, const char *filepath);
int extract_tar_to_directory(FILE *in, const char *output_dir);

#endif // FILE_IO_H