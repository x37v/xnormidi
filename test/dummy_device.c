#include "midi_device.h"
#include "midi.h"
#include <stdio.h>

MidiDevice test_device;

void send_func(uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2) {
   printf("out: %2x %2x %2x\n", byte0, byte1, byte2);
}

void default_callback(MidiDevice * device, uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   printf("default in: %2x %2x %2x\n", byte0, byte1, byte2);
}

void realtime_callback(MidiDevice * device, uint8_t byte){
   printf("realtime in: %2x\n", byte);
}

void cc_callback(MidiDevice * device, uint8_t status, uint8_t idx, uint8_t val) {
   printf("cc in: %2x %2x %2x\n", status, idx, val);
}


int main(void) {
   printf("\n\nSTART OF TEST\n\n");

   midi_init_device(&test_device);
   midi_device_set_send_func(&test_device, send_func);

   midi_send_cc(&test_device, 0, 0, 1);
   midi_send_cc(&test_device, 15, 1, 1);

   midi_register_default_callback(&test_device, default_callback);
   midi_register_realtime_callback(&test_device, realtime_callback);

   midi_input(&test_device, 3, 0xB0, 0, 1);
   midi_input(&test_device, 1, MIDI_CLOCK, 0, 0);

   //interspersed
   midi_input(&test_device, 1, 0xB0, 0, 0);
   midi_input(&test_device, 1, MIDI_CLOCK, 0, 0);
   midi_input(&test_device, 1, 0, 0, 0);
   midi_input(&test_device, 1, MIDI_START, 0, 0);
   midi_input(&test_device, 1, 1, 0, 0);

   midi_process(&test_device);
   midi_process(&test_device);

   printf("\nregistering cc callback\n");
   midi_register_cc_callback(&test_device, cc_callback);
   midi_input(&test_device, 3, 0xB0, 0, 1);
   midi_process(&test_device);
   midi_process(&test_device);

   midi_input(&test_device, 1, 0xB0, 0, 0);
   midi_input(&test_device, 1, MIDI_CLOCK, 0, 0);
   midi_input(&test_device, 1, 0, 0, 0);
   midi_input(&test_device, 1, MIDI_START, 0, 0);
   midi_input(&test_device, 1, 1, 0, 0);
   midi_process(&test_device);

   printf("\n\nEND OF TEST\n\n");
   return 0;
}
