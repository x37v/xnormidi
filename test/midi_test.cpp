#include "midi_test.h"
#include <vector>
#include <streambuf>

#include "midi.h"

using std::vector;

CPPUNIT_TEST_SUITE_REGISTRATION( MIDITest );

class CallbackInfo {
   public:
      CallbackInfo(std::string t, uint8_t byte0 = 0, uint8_t byte1 = 0, uint8_t byte2 = 0) :
         type(t) {
            bytes[0] = byte0;
            bytes[1] = byte1;
            bytes[2] = byte2;
         }
      std::string type;
      uint8_t bytes[3];
};

static vector<CallbackInfo> callback_data;

extern "C" void cc_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   callback_data.push_back(CallbackInfo("cc", byte0, byte1, byte2));
}
extern "C" void noteon_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   callback_data.push_back(CallbackInfo("noteon", byte0, byte1, byte2));
}
extern "C" void noteoff_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   callback_data.push_back(CallbackInfo("noteoff", byte0, byte1, byte2));
}
extern "C" void aftertouch_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   callback_data.push_back(CallbackInfo("aftertouch", byte0, byte1, byte2));
}
extern "C" void pitchbend_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   callback_data.push_back(CallbackInfo("pitchbend", byte0, byte1, byte2));
}
extern "C" void songposition_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   callback_data.push_back(CallbackInfo("songposition", byte0, byte1, byte2));
}

extern "C" void progchange_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1){
   callback_data.push_back(CallbackInfo("progchange", byte0, byte1));
}
extern "C" void chanpressure_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1){
   callback_data.push_back(CallbackInfo("chanpressure", byte0, byte1));
}
extern "C" void songselect_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1){
   callback_data.push_back(CallbackInfo("songselect", byte0, byte1));
}
extern "C" void tc_quarterframe_callback(MidiDevice * device, uint8_t byte0, uint8_t byte1){
   callback_data.push_back(CallbackInfo("tc_quarterframe", byte0, byte1));
}

extern "C" void realtime_callback(MidiDevice * device, uint8_t byte){
   callback_data.push_back(CallbackInfo("realtime", byte));
}
extern "C" void tunerequest_callback(MidiDevice * device, uint8_t byte){
   callback_data.push_back(CallbackInfo("tunerequest", byte));
}

extern "C" void fallthrough_callback(MidiDevice * device, uint16_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   callback_data.push_back(CallbackInfo("fallthrough", byte0, byte1, byte2));
}

extern "C" void catchall_callback(MidiDevice * device, uint16_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   callback_data.push_back(CallbackInfo("catchall", byte0, byte1, byte2));
}

extern "C" void sysex_callback(MidiDevice * device, uint16_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2){
   callback_data.push_back(CallbackInfo("catchall", byte0, byte1, byte2));
}

void MIDITest::setUp() {
   callback_data.clear();
}

void MIDITest::statusByteTest() {
   const uint8_t status[] = {
      MIDI_CLOCK,
      MIDI_TICK,
      MIDI_START,
      MIDI_CONTINUE,
      MIDI_STOP,
      MIDI_ACTIVESENSE,
      MIDI_RESET,
      MIDI_CC,
      MIDI_NOTEON,
      MIDI_NOTEOFF,
      MIDI_AFTERTOUCH,
      MIDI_PITCHBEND,
      MIDI_PROGCHANGE,
      MIDI_CHANPRESSURE,
      MIDI_TC_QUARTERFRAME,
      MIDI_SONGPOSITION,
      MIDI_SONGSELECT,
      MIDI_TUNEREQUEST,
      SYSEX_BEGIN,
      SYSEX_END
   };

   for(unsigned int i = 0; i < 0x80; i++) {
      std::stringstream msg;
      msg << "testing: " << i;
      CPPUNIT_ASSERT_MESSAGE(msg.str(), !midi_is_statusbyte(i));
   }

   for(unsigned int i = 0; i < sizeof(status); i++) {
      std::stringstream msg;
      msg << "testing: " << status[i];
      CPPUNIT_ASSERT_MESSAGE(msg.str(), midi_is_statusbyte(status[i]));
   }
}

void MIDITest::realtimeTest() {
   const uint8_t realtime[] = {
      MIDI_CLOCK,
      MIDI_TICK,
      MIDI_START,
      MIDI_CONTINUE,
      MIDI_STOP,
      MIDI_ACTIVESENSE,
      MIDI_RESET
   };
   const uint8_t not_realtime[] = {
      MIDI_CC,
      MIDI_NOTEON,
      MIDI_NOTEOFF,
      MIDI_AFTERTOUCH,
      MIDI_PITCHBEND,
      MIDI_PROGCHANGE,
      MIDI_CHANPRESSURE,
      MIDI_TC_QUARTERFRAME,
      MIDI_SONGPOSITION,
      MIDI_SONGSELECT,
      MIDI_TUNEREQUEST,
      SYSEX_BEGIN,
      SYSEX_END,
      0,
      1,
      2,
      3,
      4,
      127
   };

   for (unsigned int i = 0; i < sizeof(realtime); i++) {
      std::stringstream msg;
      msg << "testing: " << realtime[i];
      CPPUNIT_ASSERT_MESSAGE(msg.str(), midi_is_realtime(realtime[i]));
   }

   for (unsigned int i = 0; i < sizeof(not_realtime); i++) {
      std::stringstream msg;
      msg << "testing: " << not_realtime[i];
      CPPUNIT_ASSERT_MESSAGE(msg.str(), !midi_is_realtime(not_realtime[i]));
   }
}

void MIDITest::packetLengthTest() {
   const uint8_t one_byte[] = {
      MIDI_CLOCK,
      MIDI_TICK,
      MIDI_START,
      MIDI_CONTINUE,
      MIDI_STOP,
      MIDI_ACTIVESENSE,
      MIDI_RESET,
      MIDI_TUNEREQUEST
   };

   const uint8_t two_byte[] = {
      MIDI_PROGCHANGE,
      MIDI_CHANPRESSURE,
      MIDI_SONGSELECT,
      MIDI_TC_QUARTERFRAME
   };

   const uint8_t three_byte[] = {
      MIDI_CC,
      MIDI_NOTEON,
      MIDI_NOTEOFF,
      MIDI_AFTERTOUCH,
      MIDI_PITCHBEND,
      MIDI_SONGPOSITION
   };

   for(unsigned int i = 0; i < sizeof(one_byte); i++) {
      const uint8_t status = one_byte[i];
      std::stringstream msg;
      msg << "testing: " << status;
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg.str(), ONE, midi_packet_length(status));
      //test channel messages
      if (status < 0xF) {
         for (unsigned int c = 0; c <= 0xF; c++)
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg.str(), ONE, midi_packet_length(status | c));
      }
   }

   for(unsigned int i = 0; i < sizeof(two_byte); i++) {
      const uint8_t status = two_byte[i];
      std::stringstream msg;
      msg << "testing: " << status;
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg.str(), TWO, midi_packet_length(status));
      //test channel messages
      if (status < 0xF) {
         for (unsigned int c = 0; c <= 0xF; c++)
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg.str(), TWO, midi_packet_length(status | c));
      }
   }
   
   for(unsigned int i = 0; i < sizeof(three_byte); i++) {
      const uint8_t status = three_byte[i];
      std::stringstream msg;
      msg << "testing: " << status;
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg.str(), THREE, midi_packet_length(status));
      //test channel messages
      if (status < 0xF) {
         for (unsigned int c = 0; c <= 0xF; c++)
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg.str(), THREE, midi_packet_length(status | c));
      }
   }

   CPPUNIT_ASSERT_EQUAL(UNDEFINED, midi_packet_length(SYSEX_BEGIN));
   CPPUNIT_ASSERT_EQUAL(UNDEFINED, midi_packet_length(SYSEX_END));
   for(uint8_t i = 0; i < 0x80; i++)
      CPPUNIT_ASSERT_EQUAL(UNDEFINED, midi_packet_length(i));
}

#define SIZE 1024
void MIDITest::threeByteCallbacks() {
   typedef void (*three_byte_callback_reg_t)(MidiDevice * device, midi_three_byte_func_t func);

   three_byte_callback_reg_t three_byte_registrations[] = {
      midi_register_cc_callback,
      midi_register_noteon_callback,
      midi_register_noteoff_callback,
      midi_register_aftertouch_callback,
      midi_register_pitchbend_callback,
      midi_register_songposition_callback
   };

   midi_three_byte_func_t three_byte_callbacks[] = {
      cc_callback,
      noteon_callback,
      noteoff_callback,
      aftertouch_callback,
      pitchbend_callback,
      songposition_callback
   };

   uint8_t three_byte_status_bytes[] = {
      MIDI_CC,
      MIDI_NOTEON,
      MIDI_NOTEOFF,
      MIDI_AFTERTOUCH,
      MIDI_PITCHBEND,
      MIDI_SONGPOSITION
   };

   std::string three_byte_cb_names[] = {
      "cc",
      "noteon",
      "noteoff",
      "aftertouch",
      "pitchbend",
      "songposition"
   };

   for (unsigned int cb_type = 0; cb_type < sizeof(three_byte_status_bytes); cb_type++) {
      std::string msg = "failure processing: " + three_byte_cb_names[cb_type];
      callback_data.clear();

      MidiDevice device;
      midi_device_init(&device);

      uint8_t buffer[SIZE];
      buffer[0] = three_byte_status_bytes[cb_type];
      buffer[1] = 0x01;
      buffer[2] = 0x20;

      CPPUNIT_ASSERT_EQUAL(0, (int)callback_data.size());

      //nothing has been registered, should not get callback after processing
      midi_device_input(&device, 0, NULL);
      midi_device_input(&device, 3, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, (int)callback_data.size());

      //set up fall through registration
      midi_register_fallthrough_callback(&device, fallthrough_callback);
      midi_device_input(&device, 3, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, (int)callback_data.size());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, std::string("fallthrough"), callback_data.back().type);
      for(unsigned int i = 0; i < 3; i++)
         CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, buffer[i], callback_data.back().bytes[i]);

      //set up a registration
      three_byte_registrations[cb_type](&device, three_byte_callbacks[cb_type]);
      midi_device_input(&device, 3, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, (int)callback_data.size());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, three_byte_cb_names[cb_type], callback_data.back().type);
      for(unsigned int i = 0; i < 3; i++)
         CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, buffer[i], callback_data.back().bytes[i]);
   }
}

void MIDITest::twoByteCallbacks() {

   typedef void (*two_byte_callback_reg_t)(MidiDevice * device, midi_two_byte_func_t func);

   const two_byte_callback_reg_t two_byte_registrations[] = {
      midi_register_progchange_callback,
      midi_register_chanpressure_callback,
      midi_register_songselect_callback,
      midi_register_tc_quarterframe_callback
   };

   const midi_two_byte_func_t two_byte_callbacks[] = {
      progchange_callback,
      chanpressure_callback,
      songselect_callback,
      tc_quarterframe_callback
   };

   const uint8_t two_byte_status_bytes[] = {
      MIDI_PROGCHANGE,
      MIDI_CHANPRESSURE,
      MIDI_SONGSELECT,
      MIDI_TC_QUARTERFRAME
   };

   const std::string two_byte_cb_names[] = {
      "progchange",
      "chanpressure",
      "songselect",
      "tc_quarterframe"
   };

   for (unsigned int cb_type = 0; cb_type < sizeof(two_byte_status_bytes); cb_type++) {
      std::string msg = "failure processing: " + two_byte_cb_names[cb_type];
      callback_data.clear();

      MidiDevice device;
      midi_device_init(&device);

      uint8_t buffer[SIZE];
      buffer[0] = two_byte_status_bytes[cb_type];
      buffer[1] = 0x01;

      CPPUNIT_ASSERT_EQUAL(0, (int)callback_data.size());

      //nothing has been registered, should not get callback after processing
      midi_device_input(&device, 0, NULL);
      midi_device_input(&device, 2, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, (int)callback_data.size());

      //set up fall through registration
      midi_register_fallthrough_callback(&device, fallthrough_callback);
      midi_device_input(&device, 2, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, (int)callback_data.size());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, std::string("fallthrough"), callback_data.back().type);
      for(unsigned int i = 0; i < 2; i++)
         CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, buffer[i], callback_data.back().bytes[i]);

      //set up a registration
      two_byte_registrations[cb_type](&device, two_byte_callbacks[cb_type]);
      midi_device_input(&device, 2, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, (int)callback_data.size());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, two_byte_cb_names[cb_type], callback_data.back().type);
      for(unsigned int i = 0; i < 2; i++)
         CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, buffer[i], callback_data.back().bytes[i]);
   }
}

void MIDITest::oneByteCallbacks() {
   const uint8_t onebyte[] = {
      MIDI_CLOCK,
      MIDI_TICK,
      MIDI_START,
      MIDI_CONTINUE,
      MIDI_STOP,
      MIDI_ACTIVESENSE,
      MIDI_RESET
   };
   for(unsigned int i = 0; i < sizeof(onebyte); i++) {
      const uint8_t status = onebyte[i];
      std::string msg = "failure processing: " + status;
      callback_data.clear();

      MidiDevice device;
      midi_device_init(&device);

      uint8_t buffer[SIZE];
      buffer[0] = status;

      CPPUNIT_ASSERT_EQUAL(0, (int)callback_data.size());

      //nothing has been registered, should not get callback after processing
      midi_device_input(&device, 0, NULL);
      midi_device_input(&device, 1, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, (int)callback_data.size());

      //set up fall through registration
      midi_register_fallthrough_callback(&device, fallthrough_callback);
      midi_device_input(&device, 1, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, (int)callback_data.size());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, std::string("fallthrough"), callback_data.back().type);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, buffer[0], callback_data.back().bytes[0]);

      //set up a registration
      midi_register_realtime_callback(&device, realtime_callback);
      midi_device_input(&device, 1, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, (int)callback_data.size());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, std::string("realtime"), callback_data.back().type);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, buffer[0], callback_data.back().bytes[0]);
   }

   {
      const uint8_t status = MIDI_TUNEREQUEST;
      std::string msg = "failure processing: " + status;
      callback_data.clear();

      MidiDevice device;
      midi_device_init(&device);

      uint8_t buffer[SIZE];
      buffer[0] = status;

      CPPUNIT_ASSERT_EQUAL(0, (int)callback_data.size());

      //nothing has been registered, should not get callback after processing
      midi_device_input(&device, 0, NULL);
      midi_device_input(&device, 1, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, (int)callback_data.size());

      //set up fall through registration
      midi_register_fallthrough_callback(&device, fallthrough_callback);
      midi_device_input(&device, 1, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, (int)callback_data.size());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, std::string("fallthrough"), callback_data.back().type);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, buffer[0], callback_data.back().bytes[0]);

      //set up a registration
      midi_register_tunerequest_callback(&device, tunerequest_callback);
      midi_device_input(&device, 1, buffer);
      midi_device_process(&device);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, (int)callback_data.size());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, std::string("tunerequest"), callback_data.back().type);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, buffer[0], callback_data.back().bytes[0]);
   }
}
