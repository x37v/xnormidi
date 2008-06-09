//midi for avr chips,
//Copyright 2008 Alex Norman
//writen by Alex Norman with help from the Atmega16 manual (usart) and
//http://www.borg.com/~jglatt/tech/midispec.htm
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
//

#ifndef AVR_MIDI_H
#define AVR_MIDI_H
#include <inttypes.h>
#include <stdbool.h>

//midi uses the pins TX [to send] and RX [to receive] (USART)

#define MIDI_CLOCK_12MHZ_OSC 23

#define SYSEX_BEGIN 0xF0
#define SYSEX_END 0xF7
//if you and this with a byte and you get anything non-zero
//it is a status message
#define MIDI_STATUSMASK 0x80
//if you and this with a status message that contains channel info,
//you'll get the channel
#define MIDI_CHANMASK 0x0F

#define MIDI_CC 0xB0
#define MIDI_NOTEON 0x90
#define MIDI_NOTEOFF 0x80
#define MIDI_AFTERTOUCH 0xA0
#define MIDI_PITCHBEND 0xE0
#define MIDI_PROGCHANGE 0xC0
#define MIDI_CHANPRESSURE 0xD0

//midi realtime
#define MIDI_CLOCK 0xF8
#define MIDI_TICK 0xF9
#define MIDI_START 0xFA
#define MIDI_CONTINUE 0xFB
#define MIDI_STOP 0xFC
#define MIDI_ACTIVESENSE 0xFE
#define MIDI_RESET 0xFF

#define MIDI_TC_QUATERFRAME 0xF1
#define MIDI_SONGPOSITION 0xF2
#define MIDI_SONGSELECT 0xF3
#define MIDI_TUNEREQUEST 0xF6

//This ID is for educational or development use only
#define SYSEX_EDUMANUFID 0x7D

//works for ATmega16, ATmega32, ATmega323, ATmega8
#define MIDI_IN_ISR ISR(USART_RXC_vect)
#define MIDI_IN_GET_BYTE UDR

//give the UBRR value, init output, init input
//it should be set so that the baud rate is 31.25 Kbaud [+/- 1%]
void midiInit(uint16_t clockScale, bool out, bool in);
void midiSendByte(uint8_t inByte);

#ifndef MIDI_BASIC

void midiSendCC(uint8_t chan, uint8_t num, uint8_t val);
void midiSendNoteOn(uint8_t chan, uint8_t num, uint8_t vel);
void midiSendNoteOff(uint8_t chan, uint8_t num, uint8_t vel);
void midiSendAfterTouch(uint8_t chan, uint8_t note_num, uint8_t amt);
//range -8192, 8191
void midiSendPitchBend(uint8_t chan, int16_t amt);
void midiSendProgramChange(uint8_t chan, uint8_t num);
void midiSendChannelPressure(uint8_t chan, uint8_t amt);

inline void midiSendClock(void);
inline void midiSendTick(void);
inline void midiSendStart(void);
inline void midiSendContinue(void);
inline void midiSendStop(void);
inline void midiSendActiveSense(void);
inline void midiSendReset(void);

void midiTCQuaterFrame(uint8_t time);
//range 0..16383
void midiSendSongPosition(uint16_t pos);
void midiSendSongSelect(uint8_t song);
inline void midiSendTuneRequest(void);

#endif

#ifdef MIDI_BITPACKING

//utils
//this gives the number of bytes needed to pack a buffer of inSize bytes
uint8_t midiPackedSize(uint8_t inSize);
//this tells you how many bytes will result from inSize packed bytes, after unpacking
uint8_t midiUnpackedSize(uint8_t inSize);

//this packs bytes into midi sendable bytes [with he high bit set to zero]
void midiBitPack(uint8_t *outputData, uint8_t *inputData, uint8_t inDataSize);
//this unpacks 'packed' midi bytes into useable 8 bit bytes
void midiBitUnpack(uint8_t *outputData, uint8_t *inputData, uint8_t inDataSize);

#endif

#endif

