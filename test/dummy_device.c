#include "midi_device.h"
#include "midi.h"
#include <stdio.h>
#include <assert.h>

MidiDevice test_device;

uint8_t sent[3];
uint8_t got[3];

bool cc_called;
bool noteon_called;
bool noteoff_called;
bool aftertouch_called;
bool pitchbend_called;
bool songposition_called;
bool progchange_called;
bool chanpressure_called;
bool songselect_called;
bool tc_quarterframe_called;
bool realtime_called;
bool tunerequest_called;
bool default_called;

void send_func(uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2) {
   sent[0] = byte0;
   sent[1] = byte1;
   sent[2] = byte0;
}

void cc_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   cc_called = true;
   got[0] = byte0;
   got[1] = byte1;
   got[2] = byte2;
}
void noteon_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   noteon_called = true;
   got[0] = byte0;
   got[1] = byte1;
   got[2] = byte2;
}
void noteoff_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   noteoff_called = true;
   got[0] = byte0;
   got[1] = byte1;
   got[2] = byte2;
}
void aftertouch_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   aftertouch_called = true;
   got[0] = byte0;
   got[1] = byte1;
   got[2] = byte2;
}
void pitchbend_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   pitchbend_called = true;
   got[0] = byte0;
   got[1] = byte1;
   got[2] = byte2;
}
void songposition_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   songposition_called = true;
   got[0] = byte0;
   got[1] = byte1;
   got[2] = byte2;
}

void progchange_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1){
   progchange_called = true;
   got[0] = byte0;
   got[1] = byte1;
}
void chanpressure_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1){
   chanpressure_called = true;
   got[0] = byte0;
   got[1] = byte1;
}
void songselect_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1){
   songselect_called = true;
   got[0] = byte0;
   got[1] = byte1;
}
void tc_quarterframe_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1){
   tc_quarterframe_called = true;
   got[0] = byte0;
   got[1] = byte1;
}

void realtime_callback(MidiDevice * device, uint8_t byte){
   realtime_called = true;
   got[0] = byte;
}
void tunerequest_callback(MidiDevice * device, uint8_t byte){
   tunerequest_called = true;
   got[0] = byte;
}

void default_callback(MidiDevice * device, uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   default_called = true;
   got[0] = byte0;
   got[1] = byte1;
   got[2] = byte2;
}

void reset() {
   uint8_t i;
   for(i = 0; i < 3; i++)
      got[i] = sent[i] = 0;
   cc_called = false;
   noteon_called = false;
   noteoff_called = false;
   aftertouch_called = false;
   pitchbend_called = false;
   songposition_called = false;
   progchange_called = false;
   chanpressure_called = false;
   songselect_called = false;
   tc_quarterframe_called = false;
   realtime_called = false;
   tunerequest_called = false;
   default_called = false;
}

bool anything_called() {
   return cc_called ||
      noteon_called ||
      noteoff_called ||
      aftertouch_called ||
      pitchbend_called ||
      songposition_called ||
      progchange_called ||
      chanpressure_called ||
      songselect_called ||
      tc_quarterframe_called ||
      realtime_called ||
      tunerequest_called ||
      default_called;
}

int main(void) {
   midi_init_device(&test_device);
   midi_device_set_send_func(&test_device, send_func);

   midi_send_cc(&test_device, 0, 0, 1);
   midi_send_cc(&test_device, 15, 1, 1);

   midi_register_default_callback(&test_device, default_callback);
   midi_register_realtime_callback(&test_device, realtime_callback);

   assert(!default_called);
   assert(!realtime_called);
   midi_device_input(&test_device, 3, 0xB0, 0, 1);
   midi_device_input(&test_device, 1, MIDI_CLOCK, 0, 0);
   midi_process(&test_device);
   assert(default_called);
   assert(realtime_called);

   reset();
   assert(!default_called);
   assert(!realtime_called);
   //interspersed
   midi_device_input(&test_device, 1, 0xB0, 0, 0);
   midi_device_input(&test_device, 1, MIDI_CLOCK, 0, 0);
   midi_device_input(&test_device, 1, 0, 0, 0);
   midi_device_input(&test_device, 1, MIDI_START, 0, 0);
   midi_device_input(&test_device, 1, 1, 0, 0);
   midi_process(&test_device);
   assert(default_called);
   assert(realtime_called);

   midi_register_cc_callback(&test_device, cc_callback);
   assert(!cc_called);
   midi_device_input(&test_device, 3, 0xB0, 0, 1);
   midi_process(&test_device);
   assert(cc_called);

   reset();
   assert(!cc_called);
   assert(!realtime_called);
   midi_device_input(&test_device, 1, 0xB0, 0, 0);
   midi_device_input(&test_device, 1, MIDI_CLOCK, 0, 0);
   midi_device_input(&test_device, 1, 0, 0, 0);
   midi_device_input(&test_device, 1, MIDI_START, 0, 0);
   midi_device_input(&test_device, 1, 1, 0, 0);
   midi_process(&test_device);
   assert(cc_called);
   assert(realtime_called);

   reset();
   assert(!anything_called());
   midi_device_input(&test_device, 1, 0xB0, 0, 0);
   midi_device_input(&test_device, 1, 0, 0, 0);
   midi_process(&test_device);
   assert(!anything_called());

   printf("\n\nTEST PASSED!\n\n");
   return 0;
}
