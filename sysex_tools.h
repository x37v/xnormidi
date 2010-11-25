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

/**
 * These functions are for converting data to and from a "midi-safe" format,
 * which can be use to send data with sysex messages.  Sysex messages may only
 * contain data where the to bit is not set. 
 *
 * A bit "packed" midi message is one that contains all of the data from its
 * original state, but does not have any of the top bits set.
 *
 * Every 7 bytes of unpacked data is converted into 8 bytes of packed data and
 * visa-versa.  If you'd like to operate on small segments, make sure that you
 * pack in 7 byte increments and unpack in 8 byte increments.
 *
 */

/**
 * Compute the length of a message after it is packed.
 *
 * @param unpacked_length The length, in bytes, of the message to pack.
 *
 * @return The length, in bytes, of the message after packing.
 */
uint16_t sysex_bit_packed_length(uint16_t unpacked_length);

/**
 * Compute the length of a message after it is unpacked.
 *
 * @param packed_length The length, in bytes, of the packed message.
 *
 * @return The length, in bytes, of the message after it is unpacked.
 */
uint16_t sysex_bit_unpacked_length(uint16_t packed_length);

/**
 * Pack data so that it can be transmitted safely in a sysex message.
 *
 * @param packed The output data buffer, must be at least sysex_bit_packed_length(length) bytes long.
 * @param source The input buffer of data to be packed.
 * @param length The number of bytes from the input buffer to pack.
 * 
 * @return number of bytes packed.
 */
uint16_t sysex_bit_pack(uint8_t *packed, const uint8_t *source, uint16_t length);

/**
 * Unpack packed data.
 *
 * @param unpacked The output data buffer, must be at least sysex_bit_unpacked_length(length) bytes long.
 * @param source The input buffer of data to be unpacked.
 * @param length The number of bytes from the input buffer to unpack.
 * 
 * @return number of bytes unpacked.
 */
uint16_t sysex_bit_unpack(uint8_t *unpacked, const uint8_t *source, uint16_t length);

#endif
