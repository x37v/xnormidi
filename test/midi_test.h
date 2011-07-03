#ifndef MIDI_TEST_H
#define MIDI_TEST_H

#include <cppunit/extensions/HelperMacros.h>

class MIDITest : public CppUnit::TestCase { 
   public:
      CPPUNIT_TEST_SUITE( MIDITest );
      CPPUNIT_TEST( testOne );
      CPPUNIT_TEST_SUITE_END(); 

      void testOne();
};

#endif
