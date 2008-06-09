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
#include <avr/io.h>
#include <avr/interrupt.h>

void midiSendByte(uint8_t inByte){
	// Wait for empty transmit buffer
	while ( !(UCSRA & _BV(UDRE)) );
	UDR = inByte;
}

void midiInit(uint16_t clockScale, bool out, bool in){
	// Set baud rate
	UBRRH = (uint8_t)(clockScale >> 8);
	UBRRL = (uint8_t)(clockScale & 0xFF);
	// Enable transmitter
	if(out)
		UCSRB |= _BV(TXEN);
	if(in) {
		//Enable receiver
		//RX Complete Interrupt Enable  (user must provide routine)
		UCSRB |= _BV(RXEN) | _BV(RXCIE);
	}
	//Set frame format: Async, 8data, 1 stop bit, 1 start bit, no parity
	//needs to have URSEL set in order to write into this reg
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
}

#ifndef MIDI_BASIC

void midiSendCC(uint8_t chan, uint8_t num, uint8_t val){
	//CC Status: 0xB0 to 0xBF where the low nibble is the MIDI channel.
	midiSendByte(MIDI_CC | (chan & MIDI_CHANMASK));
	//CC Data: Controller Num, Controller Val
	midiSendByte(num & 0x7F);
	midiSendByte(val & 0x7F);
}

void midiSendNoteOn(uint8_t chan, uint8_t num, uint8_t vel){
	midiSendByte(MIDI_NOTEON | (chan & MIDI_CHANMASK));
	//Note Data: Note Num, Note Velocity
	midiSendByte(num & 0x7F);
	midiSendByte(vel & 0x7F);
}

void midiSendNoteOff(uint8_t chan, uint8_t num, uint8_t vel){
	midiSendByte(MIDI_NOTEOFF | (chan & MIDI_CHANMASK));
	//Note Data: Note Num, Note Velocity
	midiSendByte(num & 0x7F);
	midiSendByte(vel & 0x7F);
}

void midiSendAfterTouch(uint8_t chan, uint8_t note_num, uint8_t amt){
	midiSendByte(MIDI_AFTERTOUCH | (chan & MIDI_CHANMASK));
	midiSendByte(note_num & 0x7F);
	midiSendByte(amt & 0x7F);
}

//XXX does this work right?
//amt in range -0x2000, 0x1fff
//uAmt should be in range..
//0x0000 to 0x3FFF
void midiSendPitchBend(uint8_t chan, int16_t amt){
	uint16_t uAmt;
	//check range
	if(amt > 0x1fff){
		uAmt = 0x3FFF;
	} else if(amt < -0x2000){
		uAmt = 0;
	} else {
		uAmt = amt + 0x2000;
	}
	midiSendByte(MIDI_PITCHBEND | (chan & MIDI_CHANMASK));
	midiSendByte(uAmt & 0x7F);
	midiSendByte((uAmt >> 7) & 0x7F);
}

void midiSendProgramChange(uint8_t chan, uint8_t num){
	midiSendByte(MIDI_PROGCHANGE | (chan & MIDI_CHANMASK));
	midiSendByte(num & 0x7F);
}

void midiSendChannelPressure(uint8_t chan, uint8_t amt){
	midiSendByte(MIDI_CHANPRESSURE | (chan & MIDI_CHANMASK));
	midiSendByte(amt & 0x7F);
}

void midiSendClock(void){
	midiSendByte(MIDI_CLOCK);
}

void midiSendTick(void){
	midiSendByte(MIDI_TICK);
}

void midiSendStart(void){
	midiSendByte(MIDI_START);
}

void midiSendContinue(void){
	midiSendByte(MIDI_CONTINUE);
}

void midiSendStop(void){
	midiSendByte(MIDI_STOP);
}

void midiSendActiveSense(void){
	midiSendByte(MIDI_ACTIVESENSE);
}

void midiSendReset(void){
	midiSendByte(MIDI_RESET);
}

void midiTCQuaterFrame(uint8_t time){
	midiSendByte(MIDI_TC_QUATERFRAME);
	midiSendByte(time & 0x7F);
}

//XXX is this right?
void midiSendSongPosition(uint16_t pos){
	midiSendByte(MIDI_SONGPOSITION);
	midiSendByte(pos & 0x7F);
	midiSendByte((pos >> 7) & 0x7F);
}

void midiSendSongSelect(uint8_t song){
	midiSendByte(MIDI_SONGSELECT);
	midiSendByte(song & 0x7F);
}

void midiSendTuneRequest(void){
	midiSendByte(MIDI_TUNEREQUEST);
}

#endif

#ifdef MIDI_BITPACKING

uint8_t midiPackedSize(uint8_t inSize){
	//<< 3 == * 8
	uint16_t inBits = inSize << 3;
   //compute the number of bytes needed for the output data
   return ((uint8_t)(inBits / 7) + (uint8_t)((inBits % 7) == 0 ? 0 : 1));
}

uint8_t midiUnpackedSize(uint8_t inSize){
	uint16_t validbits = inSize * 7;
	// >> 3 == / 8
	return (uint8_t)(validbits >> 3);
}

void midiBitPack(uint8_t *outputData, uint8_t *inputData, uint8_t inDataSize){
	uint8_t i;
	uint8_t pageIndex;
	for(i = 0; i < inDataSize; i++){
		// << 3 == * 8
		pageIndex = (i / 7) << 3;
		switch(i % 7){
			case 0:
				outputData[pageIndex] = inputData[i] >> 1;
				outputData[pageIndex + 1] = (inputData[i] << 6) & 0x7F;
				break;
			case 1:
				outputData[pageIndex + 1] |= inputData[i] >> 2;
				outputData[pageIndex + 2] = (inputData[i] << 5) & 0x7F;
				break;
			case 2:
				outputData[pageIndex + 2] |= inputData[i] >> 3;
				outputData[pageIndex + 3] = (inputData[i] << 4) & 0x7F;
				break;
			case 3:
				outputData[pageIndex + 3] |= inputData[i] >> 4;
				outputData[pageIndex + 4] = (inputData[i] << 3) & 0x7F;
				break;
			case 4:
				outputData[pageIndex + 4] |= inputData[i] >> 5;
				outputData[pageIndex + 5] = (inputData[i] << 2) & 0x7F;
				break;
			case 5:
				outputData[pageIndex + 5] |= inputData[i] >> 6;
				outputData[pageIndex + 6] = (inputData[i] << 1) & 0x7F;
				break;
			case 6:
				outputData[pageIndex + 6] |= inputData[i] >> 7;
				outputData[pageIndex + 7] |= inputData[i] & 0x7F;
				break;
		}
	}
}

void midiBitUnpack(uint8_t *outputData, uint8_t *inputData, uint8_t inDataSize){
	uint8_t i;
	uint8_t pageIndex;
	for(i = 0; i < inDataSize; i++){
		//>> 3 == / 8
		pageIndex = 7 * (i >> 3);
		switch(i % 8){
			case 0:
				outputData[pageIndex] = inputData[i] << 1;
				break;
			case 1:
				outputData[pageIndex] |= inputData[i] >> 6;
				outputData[pageIndex + 1] = inputData[i] << 2;
				break;
			case 2:
				outputData[pageIndex + 1] |= inputData[i] >> 5;
				outputData[pageIndex + 2] = inputData[i] << 3;
				break;
			case 3:
				outputData[pageIndex + 2] |= inputData[i] >> 4;
				outputData[pageIndex + 3] = inputData[i] << 4;
				break;
			case 4:
				outputData[pageIndex + 3] |= inputData[i] >> 3;
				outputData[pageIndex + 4] = inputData[i] << 5;
				break;
			case 5:
				outputData[pageIndex + 4] |= inputData[i] >> 2;
				outputData[pageIndex + 5] = inputData[i] << 6;
				break;
			case 6:
				outputData[pageIndex + 5] |= inputData[i] >> 1;
				outputData[pageIndex + 6] = inputData[i] << 7;
				break;
			case 7:
				outputData[pageIndex + 6] |= inputData[i];
				break;
		}
	}
}

#endif

