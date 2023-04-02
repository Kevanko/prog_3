#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "header.h"

#define MAX_NUMS 1000000

int main() {
  const char *compress = "compressed.dat", *uncompress = "uncompressed.dat";

  write_rand(uncompress, compress, MAX_NUMS);
  uint32_t *uncom_buff = read_uncompress(uncompress);
  uint32_t *com_buff = read_compress(compress);


  for(int i = 0; i < MAX_NUMS; i++)
    if(uncom_buff[i] != com_buff[i]){
      fprintf(stderr, "Not idently");
      return 1;
    }

  free(uncom_buff);
  free(com_buff);
  return 0;
}