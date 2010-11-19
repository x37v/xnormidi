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

#ifndef AVR_MIDI_H
#define AVR_MIDI_H

#include "midi_device.h"
#include "midi_function_types.h"

typedef enum {
   UNDEFINED = 0,
   ONE = 1,
   TWO = 2,
   THREE = 3} midi_packet_length_t;

//general information [device independent] ****************
//returns true if the byte given is a midi status byte
bool midi_is_statusbyte(uint8_t theByte);
//returns true if the byte given is a realtime status byte
bool midi_is_realtime(uint8_t theByte);
//returns the length of the packet associated with the status byte given
midi_packet_length_t midi_packet_length(uint8_t status);


//initialize midi device
void midi_device_init(MidiDevice * device); // [implementation in midi_device.c]

//process input data
//you need to call this if you expect your input callbacks to be called
void midi_device_process(MidiDevice * device); // [implementation in midi_device.c]


//send functions **********************
void midi_send_cc(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t val);
void midi_send_noteon(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t vel);
void midi_send_noteoff(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t vel);
void midi_send_aftertouch(MidiDevice * device, uint8_t chan, uint8_t note_num, uint8_t amt);
void midi_send_pitchbend(MidiDevice * device, uint8_t chan, int16_t amt); //range -8192, 8191
void midi_send_programchange(MidiDevice * device, uint8_t chan, uint8_t num);
void midi_send_channelpressure(MidiDevice * device, uint8_t chan, uint8_t amt);

//realtime
void midi_send_clock(MidiDevice * device);
void midi_send_tick(MidiDevice * device);
void midi_send_start(MidiDevice * device);
void midi_send_continue(MidiDevice * device);
void midi_send_stop(MidiDevice * device);
void midi_send_activesense(MidiDevice * device);
void midi_send_reset(MidiDevice * device);

//more obscure
void midi_send_tcquaterframe(MidiDevice * device, uint8_t time); //range 0..16383
void midi_send_songposition(MidiDevice * device, uint16_t pos);
void midi_send_songselect(MidiDevice * device, uint8_t song);
void midi_send_tunerequest(MidiDevice * device);
void midi_send_byte(MidiDevice * device, uint8_t b);
void midi_send_data(MidiDevice * device, uint8_t count, uint8_t byte0, uint8_t byte1, uint8_t byte2);


//input callback registration ***********************

//three byte funcs
void midi_register_cc_callback(MidiDevice * device, midi_three_byte_func_t func);
void midi_register_noteon_callback(MidiDevice * device, midi_three_byte_func_t func);
void midi_register_noteoff_callback(MidiDevice * device, midi_three_byte_func_t func);
void midi_register_aftertouch_callback(MidiDevice * device, midi_three_byte_func_t func);
void midi_register_pitchbend_callback(MidiDevice * device, midi_three_byte_func_t func);
void midi_register_songposition_callback(MidiDevice * device, midi_three_byte_func_t func);

//two byte funcs
void midi_register_progchange_callback(MidiDevice * device, midi_two_byte_func_t func);
void midi_register_chanpressure_callback(MidiDevice * device, midi_two_byte_func_t func);
void midi_register_songselect_callback(MidiDevice * device, midi_two_byte_func_t func);
void midi_register_tc_quarterframe_callback(MidiDevice * device, midi_two_byte_func_t func);

//one byte funcs
void midi_register_realtime_callback(MidiDevice * device, midi_one_byte_func_t func);
void midi_register_tunerequest_callback(MidiDevice * device, midi_one_byte_func_t func);

//fall through, only called if a more specific callback isn't matched and called
void midi_register_fallthrough_callback(MidiDevice * device, midi_var_byte_func_t func);
//catch all, always called if registered, independent of a more specific or fallthrough call
void midi_register_catchall_callback(MidiDevice * device, midi_var_byte_func_t func);

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

