//midi for embedded chips,
//Copyright 2010 Alex Norman
//
//This file is part of avr-midi.
//
//avr-midi is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//avr-midi is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with avr-midi.  If not, see <http://www.gnu.org/licenses/>.

#include "sysex_tools.h"
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define SIZE 1028

int main(int argc, char * argv[]) {
   unsigned int i, j;
   uint8_t orig[SIZE];
   uint8_t unpacked[SIZE];
   uint8_t * packed = malloc(sizeof(uint8_t) * sysex_byte_packed_length(SIZE));
   uint8_t * packed2 = malloc(sizeof(uint8_t) * sysex_byte_packed_length(SIZE));

   for(i = 0; i < SIZE; i++)
      orig[i] = rand() & 0xFF;

   for (i = 1; i < 512; i++) {
      assert(i == sysex_byte_unpacked_length(sysex_byte_packed_length(i)));
   }

   //test byte packing
   for (i = 1; i < SIZE; i++) {
      uint16_t res_size0 = sysex_byte_pack(packed, orig, i);
      uint16_t res_size1 = sysex_byte_packed_length(i);
      //printf("packed %d == %d\n", res_size0, res_size1);
      assert(res_size0 == res_size1);
      //make sure that the top bit isn't set
      for (j = 0; j < res_size0; j++)
         assert( (packed[i] & 0x80) == 0);
      //just in case, zero out unpacked..
      memset(unpacked, SIZE, 0);
      res_size1 = sysex_byte_unpack(unpacked, packed, res_size0);
      assert(i == sysex_byte_unpacked_length(res_size0));
      //printf("unpacked %d == %d\n", res_size1, i);
      assert(res_size1 == i);
      res_size0 = memcmp(orig, unpacked, i);
      assert(0 == res_size0);
   }

   for (i = 0, j = 0; i < (SIZE - 7); i += 7, j +=8) {
      //pack in segments
      assert(8 == sysex_byte_pack(packed2 + j, orig + i, 7));
   }

   for (i = 0, j = 0; i < (SIZE - 7); i += 7, j +=8) {
      //pack in segments
      assert(7 == sysex_byte_unpack(unpacked + i, packed2 + j, 8));
   }
   //make sure the unpacked contents are the same length
   assert(0 == memcmp(unpacked, orig, SIZE - 7));
   //make sure that packing in 7 byte segments is the same as packing all at once
   assert(0 == memcmp(packed2, packed, sysex_byte_packed_length(SIZE - 7)));

   free(packed);

   printf("\n%s passed!\n\n", argv[0]);
   return 0;
}
