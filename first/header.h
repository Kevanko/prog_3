#pragma once

#include <stddef.h>
#include <stdint.h>

enum {
    MaxCodeLength = 4,
    MaxNumbers = 1000000
};

size_t encode_varint(uint32_t value, uint8_t *buf);
uint32_t decode_varint(const uint8_t **bufp);
uint32_t generate_number();
void write_rand(const char *uncompress_path, const char *compress_path,
                uint32_t count);
uint32_t *read_uncompress(const char *uncompress_path);
uint32_t *read_compress(const char *compress_path);
