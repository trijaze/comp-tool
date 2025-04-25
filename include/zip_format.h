// include/zip_format.h
#ifndef ZIP_FORMAT_H
#define ZIP_FORMAT_H

#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    uint32_t signature;
    uint16_t version;
    uint16_t flags;
    uint16_t compression;
    uint16_t mod_time;
    uint16_t mod_date;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t filename_len;
    uint16_t extra_len;
} ZipLocalFileHeader;

#define ZIP_LOCAL_FILE_SIGNATURE 0x04034b50
#pragma pack(pop)

#endif // ZIP_FORMAT_H
