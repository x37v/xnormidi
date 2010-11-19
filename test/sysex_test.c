#include "sysex_tools.h"
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define SIZE 1028

int main(int argc, char * argv[]) {
   unsigned int i;
   uint8_t orig[SIZE];
   uint8_t unpacked[SIZE];
   uint8_t * packed = malloc(sizeof(uint8_t) * sysex_byte_packed_length(SIZE));

   for(i = 0; i < SIZE; i++)
      orig[i] = rand() & 0xFF;

   for (i = 1; i < 512; i++) {
      assert(i == sysex_byte_unpacked_length(sysex_byte_packed_length(i)));
   }

   //test byte packing
   for (i = 1; i < SIZE; i++) {
      uint16_t res_size = sysex_byte_pack(packed, orig, i);
      assert(res_size == sysex_byte_packed_length(i));
      res_size = sysex_byte_unpack(unpacked, packed, res_size);
      assert(0 == memcmp(orig, unpacked, i));
   }

   free(packed);

   printf("\n%s passed!\n\n", argv[0]);
   return 0;
}
