#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "header.h"

int main() {
  const char *compress = "compressed.dat", *uncompress = "uncompressed.dat";

  write_rand(uncompress, compress, MaxNumbers);
  uint32_t *uncom_buff = read_uncompress(uncompress);
  uint32_t *com_buff = read_compress(compress);


  for(int i = 0; i < MaxNumbers; i++)
    if(uncom_buff[i] != com_buff[i]){
      fprintf(stderr, "Not identity");
      return 1;
    }
  fprintf(stdout, "Identity\n");

  free(uncom_buff);
  free(com_buff);
  return 0;
}