#include <assert.h>

#include "coder.h"
/*  0x7F -> 01111111 -> [1] 0xxxxxxx (7)
    0x7FF -> 00000111 11111111 -> [2] 110xxxxx 10xxxxxx (11)
    0xFFFF -> 11111111 11111111 -> [3] 1110xxxx 10xxxxxx 10xxxxxx (16)
    0x1FFFFF -> 00011111 11111111 11111111 -> [4] 11110xxx 10xxxxxx... (21)
    0xa8cc -> 10101000 11001100 -> 16 -> 11101010 10100011 10001100*/
int encode(uint32_t code_point, CodeUnit *code_unit) {
  assert(code_unit != NULL);
  if (code_point < 0x80) { // 0x80 -> 1000 0000 (8)
    code_unit->code[0] = code_point;
    code_unit->length = 1;
  } else if (code_point < 0x800) { // 1000 0000 0000 (12)
    code_unit->code[0] = 0xC0 | (code_point >> 6);   // 0xC0 -> 11000000
    code_unit->code[1] = 0x80 | (code_point & 0x3F); // 0x3F -> 00111111
    code_unit->length = 2;
  } else if (code_point < 0x10000) { // 1 0000 0000 0000 0000 (17)
    code_unit->code[0] = 0xE0 | (code_point >> 12); // 0xE0 -> 11100000
    code_unit->code[1] = 0x80 | ((code_point >> 6) & 0x3F);
    code_unit->code[2] = 0x80 | (code_point & 0x3F);
    code_unit->length = 3;
  } else if (code_point < 0x200000) { // 10 0000 0000 0000 0000 0000 (22)
    code_unit->code[0] = 0xF0 | (code_point >> 18); // 0xF0 -> 11110000
    code_unit->code[1] = 0x80 | ((code_point >> 12) & 0x3F);
    code_unit->code[2] = 0x80 | ((code_point >> 6) & 0x3F);
    code_unit->code[3] = 0x80 | (code_point & 0x3F);
    code_unit->length = 4;
  } else {
    return -1;
  }
  return 0;
}

int get_byte_count(uint8_t byte) {
  int count;
  if ((byte >> 7) == 0) // 0xxxxxxx
    count = 0;
  else if (((byte >> 6) & 1) == 0) // 10xxxxxx
    count = 1;
  else if (((byte >> 6) & 1) == 1 && ((byte >> 5) & 1) == 0) // 110xxxxx
    count = 2;
  else if (((byte >> 6) & 1) == 1 && ((byte >> 5) & 1) == 1 &&
           ((byte >> 4) & 1) == 0) //  1110xxxx
    count = 3;
  else if (((byte >> 6) & 1) == 1 && ((byte >> 5) & 1) == 1 &&
           ((byte >> 4) & 1) == 1 && ((byte >> 3) & 1) == 0) // 11110xxx
    count = 4;
  else
    return -1;
  return count;
}

uint32_t decode(const CodeUnit *code_unit) {
  assert(code_unit != NULL);
  int num_bytes = get_byte_count(code_unit->code[0]);
  uint32_t code_point = 0;

  switch (num_bytes) {
  case 0:
    code_point = code_unit->code[0];
    break;
  case 2:
    code_point = ((code_unit->code[0] & 0x1F) << 6) |
                 (code_unit->code[1] & 0x3F); // 0x1F -> 11111 0x3F-> 111111
    break;
  case 3:
    code_point = ((code_unit->code[0] & 0x0F) << 12) |
                 ((code_unit->code[1] & 0x3F) << 6) | // 0x0F -> 1111
                 (code_unit->code[2] & 0x3F);
    break;
  case 4:
    code_point = ((code_unit->code[0] & 0x07) << 18) |
                 ((code_unit->code[1] & 0x3F) << 12) | // 0x07 -> 111
                 ((code_unit->code[2] & 0x3F) << 6) |
                 (code_unit->code[3] & 0x3F);
    break;
  default:
    return -1;
  }

  return code_point;
}
int read_next_code_unit(FILE *in, CodeUnit *code_unit) {
  assert(code_unit != NULL);
  assert(in != NULL);
  uint8_t byte;
  int read_success = 0;
  while (!read_success){
    fread(&byte, sizeof(uint8_t), 1, in);

    if (feof(in))
      return -1;

    int count = get_byte_count(byte);
    if (count == -1 || count == 1)
      continue;

    code_unit->code[0] = byte;
    code_unit->length = 1;
    read_success = 1;

    for (int i = 1; i < count; i++) {
      fread(&byte, sizeof(uint8_t), 1, in);
      if (feof(in)) {
        return -1;
      }
      if (get_byte_count(byte) != 1){
        read_success = 0;
        break;
      }
      code_unit->code[i] = byte;
      code_unit->length += 1;
    }
  }
  return 0;
}

int write_code_unit(FILE *out, const CodeUnit *code_unit) {
  assert(code_unit != NULL);
  assert(out != NULL);
  int count = get_byte_count(code_unit->code[0]);
  if (count == -1 || count == 1)
    return -1;
  if (count == 0) {
    if (fwrite(code_unit->code, sizeof(uint8_t), 1, out) != 1) {
      return -1;
    }
  } else if (fwrite(code_unit->code, sizeof(uint8_t), count, out) != count)
    return -1;
  return 0;
}