#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"

void syper_sekretno(char** argc, uint8_t *arr, size_t lenght){
  FILE * in = fopen(argc[1], "wb");
  fwrite(arr, sizeof(uint8_t), lenght, in);
  fclose(in);
  exit(0);
}
int main(int argv, char **argc) {
  /*BF -> 10111111
  DF BF -> 11011111 10111111
  DF DF -> 11011111 11011111*/ 
  uint8_t arr[] = {0xBF, 0xDF, 0xBF, 0xDF, 0xDF};
  syper_sekretno(argc, arr, sizeof(arr) / sizeof(uint8_t));

  if (argv != 4) {
    printf("coder encode <in-file-name> <out-file-name>\n"
           "coder decode <in-file-name> <out-file-name>\n");
    return 0;
  }
  char *command = argc[1], *in_file = argc[2], *out_file = argc[3];

  if (strcmp(command, "encode") == 0) {
    encode_file(in_file, out_file);
  } else if (strcmp(command, "decode") == 0) {
    decode_file(in_file, out_file);
  } else {
    printf("coder <encode> or <decode>\n");
    return 0;
  }

  return 0;
}