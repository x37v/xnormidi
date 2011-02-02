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
   uint8_t decoded[SIZE];
   uint8_t * encoded = malloc(sizeof(uint8_t) * sysex_encoded_length(SIZE));
   uint8_t * encoded2 = malloc(sizeof(uint8_t) * sysex_encoded_length(SIZE));

   for(i = 0; i < SIZE; i++)
      orig[i] = rand() & 0xFF;

   for (i = 1; i < 512; i++) {
      assert(i == sysex_decoded_length(sysex_encoded_length(i)));
   }

   //test byte encodeing
   for (i = 1; i < SIZE; i++) {
      uint16_t res_size0 = sysex_encode(encoded, orig, i);
      uint16_t res_size1 = sysex_encoded_length(i);
      //printf("encoded %d == %d\n", res_size0, res_size1);
      assert(res_size0 == res_size1);
      //make sure that the top bit isn't set
      for (j = 0; j < res_size0; j++)
         assert( (encoded[i] & 0x80) == 0);
      //just in case, zero out decoded..
      memset(decoded, SIZE, 0);
      res_size1 = sysex_decode(decoded, encoded, res_size0);
      assert(i == sysex_decoded_length(res_size0));
      //printf("decoded %d == %d\n", res_size1, i);
      assert(res_size1 == i);
      res_size0 = memcmp(orig, decoded, i);
      assert(0 == res_size0);
   }

   for (i = 0, j = 0; i < (SIZE - 7); i += 7, j +=8) {
      //encode in segments
      assert(8 == sysex_encode(encoded2 + j, orig + i, 7));
   }

   for (i = 0, j = 0; i < (SIZE - 7); i += 7, j +=8) {
      //encode in segments
      assert(7 == sysex_decode(decoded + i, encoded2 + j, 8));
   }
   //make sure the decoded contents are the same length
   assert(0 == memcmp(decoded, orig, SIZE - 7));
   //make sure that encodeing in 7 byte segments is the same as encodeing all at once
   assert(0 == memcmp(encoded2, encoded, sysex_encoded_length(SIZE - 7)));

   free(encoded);

   printf("\n%d decoded gives %d encoded\n", 130, sysex_encoded_length(130));
   printf("\n%d encoded gives %d decoded\n", 149, sysex_decoded_length(149));

   printf("\n%s passed!\n\n", argv[0]);
   return 0;
}
