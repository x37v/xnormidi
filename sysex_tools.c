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

uint16_t sysex_byte_packed_length(uint16_t unpacked_length){
   uint8_t remainder = unpacked_length % 7;
   if (remainder)
      return (unpacked_length / 7) * 8 + remainder + 1;
   else
      return (unpacked_length / 7) * 8;
}

uint16_t sysex_byte_unpacked_length(uint16_t packed_length){
   uint8_t remainder = packed_length % 8;
   if (remainder)
      return (packed_length / 8) * 7 + remainder - 1;
   else
      return (packed_length / 8) * 7;
}

uint16_t sysex_byte_pack(uint8_t *packed, const uint8_t *source, const uint16_t length){
   uint16_t packed_full = length / 7; //number of full 8 byte sections from 7 bytes of input
   uint16_t i,j;

   //fill out the fully packed sections
   for(i = 0; i < packed_full; i++) {
      uint16_t packed_msb_idx = i * 8;
      uint16_t input_start_idx = i * 7;
      packed[packed_msb_idx] = 0;
      for(j = 0; j < 7; j++){
         uint8_t current = source[input_start_idx + j];
         packed[packed_msb_idx] |= (0x80 & current) >> (1 + j);
         packed[packed_msb_idx + 1 + j] = 0x7F & current;
      }
   }

   //fill out the rest if there is any more
   uint8_t remainder = length % 7;
   if (remainder) {
      uint16_t packed_msb_idx = packed_full * 8;
      uint16_t input_start_idx = packed_full * 7;
      packed[packed_msb_idx] = 0;
      for(j = 0; j < remainder; j++){
         uint8_t current = source[input_start_idx + j];
         packed[packed_msb_idx] |= (0x80 & current) >> (1 + j);
         packed[packed_msb_idx + 1 + j] = 0x7F & current;
      }
      return packed_msb_idx + remainder + 1;
   } else {
      return packed_full * 8;
   }
}

uint16_t sysex_byte_unpack(uint8_t *unpacked, const uint8_t *source, const uint16_t length){
   uint16_t unpacked_full = length / 8;
   uint16_t i,j;

   if (length < 2)
      return 0;

   //fill out the fully packed sections
   for(i = 0; i < unpacked_full; i++) {
      uint16_t packed_msb_idx = i * 8;
      uint16_t output_start_index = i * 7;
      for(j = 0; j < 7; j++){
         unpacked[output_start_index + j] = 0x7F & source[packed_msb_idx + j + 1];
         unpacked[output_start_index + j] |= (0x80 & (source[packed_msb_idx] << (1 + j)));
      }
   }
   uint8_t remainder = length % 8;
   if (remainder) {
      uint16_t packed_msb_idx = unpacked_full * 8;
      uint16_t output_start_index = unpacked_full * 7;
      for(j = 0; j < (remainder - 1); j++) {
         unpacked[output_start_index + j] = 0x7F & source[packed_msb_idx + j + 1];
         unpacked[output_start_index + j] |= (0x80 & (source[packed_msb_idx] << (1 + j)));
      }
      return unpacked_full * 7 + remainder - 1;
   } else {
      return unpacked_full * 7;
   }
}

