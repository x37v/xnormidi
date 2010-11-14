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

#include "bytequeue/bytequeue.h"
#define MIDI_INPUT_QUEUE_LENGTH 192

typedef enum {
   UNDEFINED = 0,
   ONE = 1,
   TWO = 2,
   THREE = 3} midi_packet_length_t;

//forward declaration
typedef struct _midi_device MidiDevice;

//the function indicates the length of the packet type it is trying to send and
//gives the bytes, all bytes beyond cnt should be ignored
typedef void (* midi_send_func_t)(uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2);
typedef void (* midi_one_byte_func_t)(MidiDevice * device, uint8_t byte);
typedef void (* midi_two_byte_func_t)(MidiDevice * device, uint8_t byte0, uint8_t byte1);
typedef void (* midi_three_byte_func_t)(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2);
typedef void (* midi_var_byte_func_t)(MidiDevice * device, uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2);

typedef enum {
   IDLE, 
   TWO_BYTE_MESSAGE = 2, 
   THREE_BYTE_MESSAGE = 3,
   SYSEX_MESSAGE} input_state_t;

//this is a struct that you create and populate in order to create a new midi
//device [be it virtual or real]
struct _midi_device {
   //output send function
	midi_send_func_t send_func;

   //input callbacks
   midi_one_byte_func_t input_realtime_callback;
   //only called if more specific callback is not matched
   midi_var_byte_func_t input_default_callback;

   //USERS SHOULD NOT MODIFY ANYTHING BELOW HERE
   //for internal input processing
   uint8_t input_buffer[3];
   input_state_t input_state;
   uint8_t input_count;

   //for queueing data between the input and the processing functions
   uint8_t input_queue_data[MIDI_INPUT_QUEUE_LENGTH];
   byteQueue_t input_queue;
};

//general information [device independent]
//returns true if the byte given is a midi status byte
inline bool midi_is_statusbyte(uint8_t theByte);
//returns true if the byte given is a realtime status byte
inline bool midi_is_realtime(uint8_t theByte);
//returns the length of the packet associated with the status byte given
inline midi_packet_length_t midi_packet_length(uint8_t status);

//initialize midi device
void midi_init_device(MidiDevice * device);

//send
inline void midi_send_byte(MidiDevice * device, uint8_t b);
void midi_send_cc(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t val);
void midi_send_noteon(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t vel);
void midi_send_noteoff(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t vel);
void midi_send_aftertouch(MidiDevice * device, uint8_t chan, uint8_t note_num, uint8_t amt); //range -8192, 8191
void midi_send_pitchbend(MidiDevice * device, uint8_t chan, int16_t amt);
void midi_send_programchange(MidiDevice * device, uint8_t chan, uint8_t num);
void midi_send_channelpressure(MidiDevice * device, uint8_t chan, uint8_t amt);

//realtime
inline void midi_send_clock(MidiDevice * device);
inline void midi_send_tick(MidiDevice * device);
inline void midi_send_start(MidiDevice * device);
inline void midi_send_continue(MidiDevice * device);
inline void midi_send_stop(MidiDevice * device);
inline void midi_send_activesense(MidiDevice * device);
inline void midi_send_reset(MidiDevice * device);

//more obscure
void midi_send_tcquaterframe(MidiDevice * device, uint8_t time); //range 0..16383
void midi_send_songposition(MidiDevice * device, uint16_t pos);
void midi_send_songselect(MidiDevice * device, uint8_t song);
inline void midi_send_tunerequest(MidiDevice * device);

//processing
void midi_process(MidiDevice * device);

//input processing, only used if you're creating a custom device
void midi_input(MidiDevice * device, uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2);

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

#endif

