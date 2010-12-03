#include "midi_usb.h"

//this example simply echos midi back, for now

void fallthrough_callback(MidiDevice * device, uint16_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   midi_send_data(device, cnt, byte0, byte1, byte2);
}

int main(void) {
   MidiDevice usb_midi;

   midi_usb_init(&usb_midi);
   midi_register_fallthrough_callback(&usb_midi, fallthrough_callback);

   while(1){
      midi_device_process(&usb_midi);
   }

   return 0;
}
