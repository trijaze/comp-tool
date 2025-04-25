// src/utils/crc32.c
#include "../../include/crc32.h"
#include "../../include/logger.h"

static uint32_t crc_table[256];

void init_crc32(void) {
    uint32_t poly = 0xEDB88320;
    for (int i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            crc = (crc & 1) ? (crc >> 1) ^ poly : crc >> 1;
        }
        crc_table[i] = crc;
    }
}

uint32_t compute_crc32(const uint8_t *data, size_t len) {
    if (!data) {
        log_error("Null data in compute_crc32\n");
        return 0;
    }
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; i++) {
        crc = crc_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}