#include <inttypes.h>

#include "coder.h"

int encode_file(const char *in_file_name, const char *out_file_name) {
  FILE *in_file = fopen(in_file_name, "rb");
  FILE *out_file = fopen(out_file_name, "wb");
  if (!in_file || !out_file)
    return -1;

  uint32_t code_point;
  CodeUnit code_unit;
  while (!feof(in_file)) {
    fscanf(in_file, "%" SCNx32, &code_point);
    if (encode(code_point, &code_unit) == -1) {
      continue;
    }
    fwrite(code_unit.code, sizeof(uint8_t), code_unit.length, out_file);
  }
  
  fclose(in_file);
  fclose(out_file);
  return 0;
}
int decode_file(const char *in_file_name, const char *out_file_name) {
  FILE *in_file = fopen(in_file_name, "rb");
  FILE *out_file = fopen(out_file_name, "wb");
  if (!in_file || !out_file)
    return -1;

  fclose(in_file);
  fclose(out_file);
  return 0;
}