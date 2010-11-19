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

#ifndef SYSEX_TOOLS_H
#define SYSEX_TOOLS_H

#include <inttypes.h>

uint16_t sysex_byte_packed_length(uint16_t unpacked_length);
uint16_t sysex_byte_unpacked_length(uint16_t packed_length);

/**
* @param packed The output data buffer, must be at least sysex_byte_packed_length(length) bytes long.
* @param source The input buffer of data to be packed.
* @param length The number of bytes from the input buffer to pack.
* 
* @return number of bytes packed
*/
uint16_t sysex_byte_pack(uint8_t *packed, const uint8_t *source, uint16_t length);
uint16_t sysex_byte_unpack(uint8_t *unpacked, const uint8_t *source, uint16_t length);

#endif
