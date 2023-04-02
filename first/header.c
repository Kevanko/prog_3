#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "header.h"

size_t encode_varint(uint32_t value, uint8_t *buf) {
  assert(buf != NULL);
  uint8_t *cur = buf;
  while (value >= 0x80) {
    const uint8_t byte = (value & 0x7f) | 0x80;
    *cur = byte;
    value >>= 7;
    ++cur;
  }
  *cur = value;
  ++cur;
  return cur - buf;
}

uint32_t decode_varint(const uint8_t **bufp) {
  const uint8_t *cur = *bufp;
  uint8_t byte = *cur++;
  uint32_t value = byte & 0x7f;
  size_t shift = 7;
  while (byte >= 0x80) {
    byte = *cur++;
    value += (byte & 0x7f) << shift;
    shift += 7;
  }
  *bufp = cur;
  return value;
}
/*
 * Диапазон             Вероятность
 * -------------------- -----------
 * [0; 128)             90%
 * [128; 16384)         5%
 * [16384; 2097152)     4%
 * [2097152; 268435455) 1%
 */
uint32_t generate_number() {
  const int r = rand();
  const int p = r % 100;
  if (p < 90) {
    return r % 128;
  }
  if (p < 95) {
    return r % 16384;
  }
  if (p < 99) {
    return r % 2097152;
  }
  return r % 268435455;
}

void write_rand(const char *uncompress_path, const char *compress_path,
                uint32_t count) {
  FILE *uncompress = fopen(uncompress_path, "wb");
  FILE *compress = fopen(compress_path, "wb");

  if (!uncompress || !compress)
    return;

  for (int i = 0; i < count; i++) {
    uint32_t num = generate_number();
    uint8_t buff[MaxCodeLength] = {0};
    size_t size = encode_varint(num, buff);

    fwrite(&num, sizeof(uint32_t), 1, uncompress);
    fwrite(buff, sizeof(uint8_t), size, compress);
  }
  fclose(compress);
  fclose(uncompress);
}

uint32_t *read_uncompress(const char *uncompress_path) {
  FILE *uncompress = fopen(uncompress_path, "rb");
  if (!uncompress)
    return NULL;

  fseek(uncompress, 0, SEEK_END);
  size_t size = ftell(uncompress);
  fseek(uncompress, 0, SEEK_SET);

  uint32_t *buff = (uint32_t *)malloc(size);
  if (!buff)
    return NULL;

  fread(buff, sizeof(uint32_t), size / sizeof(uint32_t), uncompress);
  fclose(uncompress);
  return buff;
}

uint32_t *read_compress(const char *compress_path) {
  FILE *compress = fopen(compress_path, "rb");
  if (!compress)
    return NULL;

  fseek(compress, 0, SEEK_END);
  size_t size = ftell(compress);
  fseek(compress, 0, SEEK_SET);

  uint8_t *buff = (uint8_t *)malloc(size);
  const uint8_t *cur = buff;
  if (!buff)
    return NULL;

  fread(buff, sizeof(uint8_t), size / sizeof(uint8_t), compress);
  fclose(compress);

  uint32_t *result = (uint32_t *)malloc(size * sizeof(uint32_t));
  int i = 0;
  while (cur < buff + size / sizeof(uint8_t)) {
    result[i] = decode_varint(&cur);
    i += 1;
  }
  free(buff);
  return result;
}
