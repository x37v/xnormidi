/*
two-channel, buffered serial MIDI extension for XNOR-MIDI
for use with two-USART AVR's, like ATMEGA128 or ATMEGA2561 etc....
based on Alex Norman's serial_midi library
extended by Peter Thiering 2013
distributed by the GNU-GPL public licensing conditions

usage: include with the XNOR-MIDI libraries, and use similar than the original functions
don't forget to change the XNOR-MIDI and bytequeue files path!

don't forget to include the receive ISR's in Your program!

example:

ISR(USART0_RX_vect) {
  uint8_t inByte;
  inByte = UDR0;
  midi_device_input(serialMidi_device_usart0, 1, &inByte);
}

ISR(USART1_RX_vect) {
  uint8_t inByte;
  inByte = UDR1;
  midi_device_input(serialMidi_device_usart1, 1, &inByte);
}

initialization example:

serialMidi_device_usart0 = serial_midi_init_usart0(TURBO_CLOCK_RATE, true, true);
serialMidi_device_usart1 = serial_midi_init_usart1(MIDI_CLOCK_RATE, true, true);

*/


#ifndef SERIAL_MIDI_H
#define SERIAL_MIDI_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <inttypes.h>
#include "../../../midi.h"
#include "../../../bytequeue/bytequeue.h"
#define MIDI_OUTPUT_QUEUE_LENGTH 192
 
//initialize serial midi and return the device pointer
MidiDevice* serial_midi_init_usart0(uint16_t clockScale, bool out, bool in);
MidiDevice* serial_midi_init_usart1(uint16_t clockScale, bool out, bool in);

#ifdef __cplusplus
}
#endif 

#endif
