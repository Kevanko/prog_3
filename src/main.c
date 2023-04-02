#include <stdio.h>
#include <string.h>

#include "command.h"

int main(int argv, char **argc) {
  if (argv != 4) {
    printf("coder encode <in-file-name> <out-file-name>\n"
           "coder decode <in-file-name> <out-file-name>\n");
    return 0;
  }
  char *command = argc[1], *in_file = argc[2], *out_file = argc[3];

  if(strcmp(command, "encode") != 0 && strcmp(command, "decode") != 0){
    printf("coder <encode> or <decode>\n");
    return 0;
  }

  return 0;
}