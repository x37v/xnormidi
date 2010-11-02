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

#include "midi.h"

void midi_send_cc(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t val){
	//CC Status: 0xB0 to 0xBF where the low nibble is the MIDI channel.
	//CC Data: Controller Num, Controller Val
	device->three_byte_func(
			MIDI_CC | (chan & MIDI_CHANMASK),
			num & 0x7F,
			val & 0x7F);
}

void midi_send_noteon(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t vel){
	//Note Data: Note Num, Note Velocity
	device->three_byte_func(
			MIDI_NOTEON | (chan & MIDI_CHANMASK),
			num & 0x7F,
			vel & 0x7F);
}

void midi_send_noteoff(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t vel){
	//Note Data: Note Num, Note Velocity
	device->three_byte_func(
			MIDI_NOTEOFF | (chan & MIDI_CHANMASK),
			num & 0x7F,
			vel & 0x7F);
}

void midi_send_aftertouch(MidiDevice * device, uint8_t chan, uint8_t note_num, uint8_t amt){
	device->three_byte_func(
			MIDI_AFTERTOUCH | (chan & MIDI_CHANMASK),
			note_num & 0x7F,
			amt & 0x7F);
}

//XXX does this work right?
//amt in range -0x2000, 0x1fff
//uAmt should be in range..
//0x0000 to 0x3FFF
void midi_send_pitchbend(MidiDevice * device, uint8_t chan, int16_t amt){
	uint16_t uAmt;
	//check range
	if(amt > 0x1fff){
		uAmt = 0x3FFF;
	} else if(amt < -0x2000){
		uAmt = 0;
	} else {
		uAmt = amt + 0x2000;
	}
	device->three_byte_func(
			MIDI_PITCHBEND | (chan & MIDI_CHANMASK),
			uAmt & 0x7F,
			(uAmt >> 7) & 0x7F);
}

void midi_send_programchange(MidiDevice * device, uint8_t chan, uint8_t num){
	device->two_byte_func(
			MIDI_PROGCHANGE | (chan & MIDI_CHANMASK),
			num & 0x7F);
}

void midi_send_channelpressure(MidiDevice * device, uint8_t chan, uint8_t amt){
	device->two_byte_func(
			MIDI_CHANPRESSURE | (chan & MIDI_CHANMASK),
			amt & 0x7F);
}

void midi_send_clock(MidiDevice * device){
	device->one_byte_func(MIDI_CLOCK);
}

void midi_send_tick(MidiDevice * device){
	device->one_byte_func(MIDI_TICK);
}

void midi_send_start(MidiDevice * device){
	device->one_byte_func(MIDI_START);
}

void midi_send_continue(MidiDevice * device){
	device->one_byte_func(MIDI_CONTINUE);
}

void midi_send_stop(MidiDevice * device){
	device->one_byte_func(MIDI_STOP);
}

void midi_send_activesense(MidiDevice * device){
	device->one_byte_func(MIDI_ACTIVESENSE);
}

void midi_send_reset(MidiDevice * device){
	device->one_byte_func(MIDI_RESET);
}

void midi_send_tcquaterframe(MidiDevice * device, uint8_t time){
	device->two_byte_func(
			MIDI_TC_QUATERFRAME,
			time & 0x7F);
}

//XXX is this right?
void midi_send_songposition(MidiDevice * device, uint16_t pos){
	device->three_byte_func(
			MIDI_SONGPOSITION,
			pos & 0x7F,
			(pos >> 7) & 0x7F);
}

void midi_send_songselect(MidiDevice * device, uint8_t song){
	device->two_byte_func(
			MIDI_SONGSELECT,
			song & 0x7F);
}

void midi_send_tunerequest(MidiDevice * device){
	device->one_byte_func(MIDI_TUNEREQUEST);
}

