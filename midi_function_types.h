#ifndef MIDI_FUNCTION_TYPES_H
#define MIDI_FUNCTION_TYPES_H

#include <inttypes.h>
#include <stdbool.h>

//forward declaration
typedef struct _midi_device MidiDevice;

typedef void (* midi_one_byte_func_t)(MidiDevice * device, uint8_t byte);
typedef void (* midi_two_byte_func_t)(MidiDevice * device, uint8_t byte0, uint8_t byte1);
typedef void (* midi_three_byte_func_t)(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2);
//all bytes after count bytes should be ignored
typedef void (* midi_var_byte_func_t)(MidiDevice * device, uint8_t count, uint8_t byte0, uint8_t byte1, uint8_t byte2);

#endif
