#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "midi.h"

#define MIDI_CLOCK_RATE MIDI_CLOCK_12MHZ_OSC

//the midi input interrupt handler
MIDI_IN_ISR {
	uint8_t inByte;
	inByte = MIDI_IN_GET_BYTE;
	//visually indicate that the stuff happened
	PORTB = 0;
	//echo
	midiSendByte(inByte);
}

int main(void) {

	DDRB = 0xFF;
	PORTB = 0xF0;

	//init midi, give the clock rate setting, indicate that we want both input
	//and output set up
	midiInit(MIDI_CLOCK_RATE, true, true);

	//send a CC message on channel 1, cc # 12, value 3
	midiSendCC(0,12,3);

	//enable interrupts
	sei();

	//loop, we're just echoing
	while(1);

	return 0;
}

