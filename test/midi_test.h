#ifndef MIDI_TEST_H
#define MIDI_TEST_H

#include <cppunit/extensions/HelperMacros.h>

class MIDITest : public CppUnit::TestCase { 

   CPPUNIT_TEST_SUITE( MIDITest );
   CPPUNIT_TEST( statusByteTest );
   CPPUNIT_TEST( realtimeTest );
   CPPUNIT_TEST( packetLengthTest );
   CPPUNIT_TEST( threeByteCallbacks );
   CPPUNIT_TEST_SUITE_END(); 

   public:
      void statusByteTest();
      void realtimeTest();
      void packetLengthTest();
      void threeByteCallbacks();
      virtual void setUp();
};

#endif
