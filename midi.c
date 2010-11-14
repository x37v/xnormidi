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
#define MIN(x,y) (((x) < (y)) ? (x) : (y)) 

//internally used to call the callbacks
void midi_input_callbacks(MidiDevice * device, uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2);
void midi_process_byte(MidiDevice * device, uint8_t input);

bool midi_is_statusbyte(uint8_t theByte){
   return (bool)(theByte & MIDI_STATUSMASK);
}

bool midi_is_realtime(uint8_t theByte){
   return (theByte >= MIDI_CLOCK);
}

midi_packet_length_t midi_packet_length(uint8_t status){
   switch(status & MIDI_STATUSMASK){
      case MIDI_CC:
      case MIDI_NOTEON:
      case MIDI_NOTEOFF:
      case MIDI_AFTERTOUCH:
      case MIDI_PITCHBEND:
      case MIDI_SONGPOSITION:
         return THREE;
      case MIDI_PROGCHANGE:
      case MIDI_CHANPRESSURE:
      case MIDI_SONGSELECT:
      case MIDI_TC_QUATERFRAME:
         return TWO;
      case MIDI_CLOCK:
      case MIDI_TICK:
      case MIDI_START:
      case MIDI_CONTINUE:
      case MIDI_STOP:
      case MIDI_ACTIVESENSE:
      case MIDI_RESET:
      case MIDI_TUNEREQUEST:
         return ONE;
      case SYSEX_END:
      case SYSEX_BEGIN:
      default:
         return UNDEFINED;
   }
}

void midi_init_device(MidiDevice * device){
   device->input_state = IDLE;
   device->input_count = 0;
   bytequeue_init(&device->input_queue, device->input_queue_data, MIDI_INPUT_QUEUE_LENGTH);
}

void midi_send_byte(MidiDevice * device, uint8_t b){
	device->send_func(1, b, 0, 0);
}

void midi_send_cc(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t val){
	//CC Status: 0xB0 to 0xBF where the low nibble is the MIDI channel.
	//CC Data: Controller Num, Controller Val
	device->send_func(3,
			MIDI_CC | (chan & MIDI_CHANMASK),
			num & 0x7F,
			val & 0x7F);
}

void midi_send_noteon(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t vel){
	//Note Data: Note Num, Note Velocity
	device->send_func(3,
			MIDI_NOTEON | (chan & MIDI_CHANMASK),
			num & 0x7F,
			vel & 0x7F);
}

void midi_send_noteoff(MidiDevice * device, uint8_t chan, uint8_t num, uint8_t vel){
	//Note Data: Note Num, Note Velocity
	device->send_func(3,
			MIDI_NOTEOFF | (chan & MIDI_CHANMASK),
			num & 0x7F,
			vel & 0x7F);
}

void midi_send_aftertouch(MidiDevice * device, uint8_t chan, uint8_t note_num, uint8_t amt){
	device->send_func(3,
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
	device->send_func(3,
			MIDI_PITCHBEND | (chan & MIDI_CHANMASK),
			uAmt & 0x7F,
			(uAmt >> 7) & 0x7F);
}

void midi_send_programchange(MidiDevice * device, uint8_t chan, uint8_t num){
	device->send_func(2,
			MIDI_PROGCHANGE | (chan & MIDI_CHANMASK),
			num & 0x7F,
         0);
}

void midi_send_channelpressure(MidiDevice * device, uint8_t chan, uint8_t amt){
	device->send_func(2,
			MIDI_CHANPRESSURE | (chan & MIDI_CHANMASK),
			amt & 0x7F,
         0);
}

void midi_send_clock(MidiDevice * device){
	device->send_func(1, MIDI_CLOCK, 0, 0);
}

void midi_send_tick(MidiDevice * device){
	device->send_func(1, MIDI_TICK, 0, 0);
}

void midi_send_start(MidiDevice * device){
	device->send_func(1, MIDI_START, 0, 0);
}

void midi_send_continue(MidiDevice * device){
	device->send_func(1, MIDI_CONTINUE, 0, 0);
}

void midi_send_stop(MidiDevice * device){
	device->send_func(1, MIDI_STOP, 0, 0);
}

void midi_send_activesense(MidiDevice * device){
	device->send_func(1, MIDI_ACTIVESENSE, 0, 0);
}

void midi_send_reset(MidiDevice * device){
	device->send_func(1, MIDI_RESET, 0, 0);
}

void midi_send_tcquaterframe(MidiDevice * device, uint8_t time){
	device->send_func(2,
			MIDI_TC_QUATERFRAME,
			time & 0x7F,
         0);
}

//XXX is this right?
void midi_send_songposition(MidiDevice * device, uint16_t pos){
	device->send_func(3,
			MIDI_SONGPOSITION,
			pos & 0x7F,
			(pos >> 7) & 0x7F);
}

void midi_send_songselect(MidiDevice * device, uint8_t song){
	device->send_func(2,
			MIDI_SONGSELECT,
			song & 0x7F,
         0);
}

void midi_send_tunerequest(MidiDevice * device){
	device->send_func(1, MIDI_TUNEREQUEST, 0, 0);
}

void midi_input(MidiDevice * device, uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2) {
   //temp storage so we can loop over the bytes
   uint8_t input[3];
   input[0] = byte0;
   input[1] = byte1;
   input[2] = byte2;
   
   //clamp range
   if (cnt > 3)
      cnt = 3;

   uint8_t i;
   for (i = 0; i < cnt; i++)
      bytequeue_enqueue(&device->input_queue, input[i]);
}

void midi_process(MidiDevice * device) {
   //pull stuff off the queue and process
   byteQueueIndex_t len = bytequeue_length(&device->input_queue);
   uint16_t i;
   //TODO limit number of bytes processed?
   for(i = 0; i < len; i++) {
      midi_process_byte(device, bytequeue_get(&device->input_queue, 0));
      bytequeue_remove(&device->input_queue, 1);
   }
}

void midi_process_byte(MidiDevice * device, uint8_t input) {
   if (midi_is_realtime(input)) {
      //call callback, don't change any state
      midi_input_callbacks(device, 1, input, 0, 0);
   } else if (midi_is_statusbyte(input)) {
      //store the byte
      if (device->input_state != SYSEX_MESSAGE) {
         device->input_buffer[0] = input;
         device->input_count = 1;
      }
      switch (midi_packet_length(input)) {
         case ONE:
            device->input_state = IDLE;
            midi_input_callbacks(device, 1, input, 0, 0);
            break;
         case TWO:
            device->input_state = TWO_BYTE_MESSAGE;
            break;
         case THREE:
            device->input_state = THREE_BYTE_MESSAGE;
            break;
         case UNDEFINED:
            switch(input) {
               case SYSEX_BEGIN:
                  device->input_state = SYSEX_MESSAGE;
                  break;
               case SYSEX_END:
                  //failsafe, should never happen
                  if(device->input_count > 2) {
                     device->input_state = IDLE;
                     device->input_count = 2;
                  }
                  //send what is left in the input buffer, set idle
                  device->input_buffer[device->input_count] = input;
                  device->input_state = IDLE;
                  device->input_count += 1;
                  //call the callback
                  midi_input_callbacks(device, device->input_count, 
                        device->input_buffer[0], device->input_buffer[1], device->input_buffer[2]);
                  break;
               default:
                  device->input_state = IDLE;
                  device->input_count = 0;
            }

            break;
         default:
            device->input_state = IDLE;
            device->input_count = 0;
            break;
      }
   } else {
      //failsafe, should never happen
      if(device->input_state > 2)
         device->input_state = IDLE;

      if (device->input_state != IDLE) {
         //store the byte
         device->input_buffer[device->input_count] = input;
         //increment count
         device->input_count += 1;
         switch(device->input_count) {
            case 3:
               if (device->input_state != SYSEX_MESSAGE)
                  device->input_state = IDLE;
               device->input_count = 0;
               //call callback
               midi_input_callbacks(device, 3,
                     device->input_buffer[0], device->input_buffer[1], device->input_buffer[2]);
               break;
            case 2:
               if (device->input_state == TWO_BYTE_MESSAGE) {
                  device->input_state = IDLE;
                  //call callback
                  midi_input_callbacks(device, 2, device->input_buffer[0], device->input_buffer[1], 0);
               }
               break;
            case 1:
            default:
               //shouldn't happen
               device->input_state = IDLE;
               break;
         }
      }
   }
}

void midi_input_callbacks(MidiDevice * device, uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2) {
}

