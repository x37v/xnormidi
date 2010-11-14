#ifndef MIDI_DEVICE_H
#define MIDI_DEVICE_H

#include "midi_function_types.h"
#include "bytequeue/bytequeue.h"
#define MIDI_INPUT_QUEUE_LENGTH 192

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

   //********input callbacks
   //three byte funcs
   midi_three_byte_func_t input_cc_callback;
   midi_three_byte_func_t input_noteon_callback;
   midi_three_byte_func_t input_noteoff_callback;
   midi_three_byte_func_t input_aftertouch_callback;
   midi_three_byte_func_t input_pitchbend_callback;
   midi_three_byte_func_t input_songposition_callback;
   //two byte funcs
   midi_two_byte_func_t input_progchange_callback;
   midi_two_byte_func_t input_chanpressure_callback;
   midi_two_byte_func_t input_songselect_callback;
   midi_two_byte_func_t input_tc_quaterframe_callback;
   //one byte funcs
   midi_one_byte_func_t input_realtime_callback;
   midi_one_byte_func_t input_tunerequest_callback;

   //default func
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

//initialize midi device
void midi_init_device(MidiDevice * device);

//processing
void midi_process(MidiDevice * device);

//input processing, only used if you're creating a custom device
void midi_input(MidiDevice * device, uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2);
//set send function, only used if you're creating a custom device
void midi_device_set_send_func(MidiDevice * device, midi_send_func_t send_func);

#endif
