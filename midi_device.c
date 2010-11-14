#include "midi_device.h"
#include "midi.h"

#ifndef NULL
#define NULL 0
#endif

//forward declarations, internally used to call the callbacks
void midi_input_callbacks(MidiDevice * device, uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2);
void midi_process_byte(MidiDevice * device, uint8_t input);

void midi_init_device(MidiDevice * device){
   device->input_state = IDLE;
   device->input_count = 0;
   bytequeue_init(&device->input_queue, device->input_queue_data, MIDI_INPUT_QUEUE_LENGTH);

   device->input_realtime_callback = NULL;
   device->input_default_callback = NULL;
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

void midi_device_set_send_func(MidiDevice * device, midi_send_func_t send_func){
   device->send_func = send_func;
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
   //did we end up calling a callback?
   bool called = false;
   switch (cnt) {
      case 3:
         break;
      case 2:
         break;
      case 1:
         if (midi_is_realtime(byte0)) {
            if (device->input_realtime_callback) {
               device->input_realtime_callback(device, byte0);
               called = true;
            }
         }
         break;
      default:
         //just in case
         if (cnt > 3)
            cnt = 0;
         break;
   }

   //if there is a default callback and we haven't called a more specific one, 
   //call the default
   if (!called && device->input_default_callback)
      device->input_default_callback(device, cnt, byte0, byte1, byte2);
}

