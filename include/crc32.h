// include/crc32.h
#ifndef CRC32_H
#define CRC32_H

#include <stddef.h>
#include <stdint.h>

void init_crc32(void);
uint32_t compute_crc32(const uint8_t *data, size_t length);

#endif // CRC32_H