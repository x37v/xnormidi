#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "midi.h"

#define MIDI_CLOCK_RATE MIDI_CLOCK_12MHZ_OSC

//count the number of bytes received
volatile uint8_t cnt;

//the midi input interrupt handler
MIDI_IN_ISR {
	uint8_t inByte;
	inByte = MIDI_IN_GET_BYTE;
	cnt++;
	PORTB = ~cnt;
	//echo
	midiSendByte(inByte);
}

int main(void) {

	cnt = 0;

	//PORTB is an output.. displays number of bytes received
	DDRB = 0xFF;
	PORTB = ~cnt;

	//init midi, give the clock rate setting, indicate that we want both input
	//and output set up
	midiInit(MIDI_CLOCK_RATE, true, true);

	//enable interrupts
	sei();

	//busy loop, we're just echoing in the interrupt
	while(1);

	return 0;
}

