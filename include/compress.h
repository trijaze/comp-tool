// include/compress.h
#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdint.h>

// Hàm nén và giải nén trả về 0 nếu thành công, mã lỗi nếu thất bại
int zip_compress(const char *input_path, const char *output_file);
int zip_decompress(const char *zip_file, const char *output_path);

int huffman_compress(const char *input_file, const char *output_file);
int huffman_decompress(const char *input_file, const char *output_file);

int rle_compress(const char *input_file, const char *output_file);
int rle_decompress(const char *input_file, const char *output_file);

#endif // COMPRESS_H