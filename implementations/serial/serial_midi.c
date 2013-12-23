#include "serial_midi.h"
#include <avr/interrupt.h>
#include "stdlib.h"

static MidiDevice s_midi_device_usart0;
static MidiDevice s_midi_device_usart1;

byteQueue_t queue_usart0;
byteQueue_t queue_usart1;
uint8_t output_queue_data_usart0[MIDI_OUTPUT_QUEUE_LENGTH]; 
uint8_t output_queue_data_usart1[MIDI_OUTPUT_QUEUE_LENGTH]; 

// transmit interrupts
void serial_midi_send_usart1(MidiDevice * device, uint16_t cnt, uint8_t inByte0, uint8_t inByte1, uint8_t inByte2) {
  bytequeue_enqueue(&queue_usart1, inByte0); 
  if(cnt > 1)
    bytequeue_enqueue(&queue_usart1, inByte1); 
  if(cnt == 3)
    bytequeue_enqueue(&queue_usart1, inByte2); 

  // then turn on data transmit buffer interrupt
  UCSR1B |= (1 << UDRIE1); 

  /*   
  //we always send the first byte
  while ( !(UCSR1A & _BV(UDRE1)) ); // Wait for empty transmit buffer
  UDR1 = inByte0;
  //if cnt == 2 or 3 we send the send byte
  if(cnt > 1) {
  while ( !(UCSR1A & _BV(UDRE1)) ); // Wait for empty transmit buffer
  UDR1 = inByte1;
  }
  //if cnt == 3 we send the third byte
  if(cnt == 3) {
  while ( !(UCSR1A & _BV(UDRE1)) ); // Wait for empty transmit buffer
  UDR1 = inByte2;
  }
  */ 
}

void serial_midi_send_usart0(MidiDevice * device, uint16_t cnt, uint8_t inByte0, uint8_t inByte1, uint8_t inByte2) {
  // enqueue into buffer & start interrupt
  bytequeue_enqueue(&queue_usart0, inByte0); 
  if(cnt > 1)
    bytequeue_enqueue(&queue_usart0, inByte1); 
  if(cnt == 3)
    bytequeue_enqueue(&queue_usart0, inByte2); 

  // then turn on data transmit buffer interrupt
  UCSR0B |= (1 << UDRIE0); 

  /*   
  //we always send the first byte
  while ( !(UCSR0A & _BV(UDRE0)) ); // Wait for empty transmit buffer
  UDR0 = inByte0;
  //if cnt == 2 or 3 we send the send byte
  if(cnt > 1) {
  while ( !(UCSR0A & _BV(UDRE0)) ); // Wait for empty transmit buffer
  UDR0 = inByte1;
  }
  //if cnt == 3 we send the third byte
  if(cnt == 3) {
  while ( !(UCSR0A & _BV(UDRE0)) ); // Wait for empty transmit buffer
  UDR0 = inByte2;
  }
*/  
}


ISR(USART0_UDRE_vect) {
  uint8_t val;

  // check if bytes are available for transmission

  if (bytequeue_length(&queue_usart0) > 0) {
    //first, grab a byte
    val = bytequeue_get(&queue_usart0, 0); 

    //then transmit
    //and remove from queue
    UDR0 = val;
    bytequeue_remove(&queue_usart0, 1); 
  }

  // if queue is empty, stop!
  if(bytequeue_length(&queue_usart0) == 0)
    UCSR0B &= ~( 1 << UDRIE0 );
}


ISR(USART1_UDRE_vect) {
  uint8_t val2;

  // check if bytes are available for transmission

  if (bytequeue_length(&queue_usart1) > 0) {
    // first, grab a byte
    val2 = bytequeue_get(&queue_usart1, 0); 

    // then transmit
    // and remove from queue
    UDR1 = val2;
    bytequeue_remove(&queue_usart1, 1); 
  } else {
    // if queue is empty, stop!
    UCSR1B &= ~( 1 << UDRIE1 );
  }
}

MidiDevice* serial_midi_init_usart0(uint16_t clockScale, bool out, bool in) {
  //send up the device
  midi_device_init(&s_midi_device_usart0);
  bytequeue_init(&queue_usart0, output_queue_data_usart0, MIDI_OUTPUT_QUEUE_LENGTH); 

  midi_device_set_send_func(&s_midi_device_usart0, serial_midi_send_usart0);

  // Set baud rate
  UBRR0H = (uint8_t)(clockScale >> 8);
  UBRR0L = (uint8_t)(clockScale & 0xFF);
  // Enable transmitter
  if(out)
    UCSR0B |= _BV(TXEN0);
  if(in) {
    //Enable receiver
    //RX Complete Interrupt Enable  (user must provide routine)
    UCSR0B |= _BV(RXEN0) | _BV(RXCIE0);
  }
  //Set frame format: Async, 8data, 1 stop bit, 1 start bit, no parity
  //needs to have URSEL set in order to write into this reg
  // UCSR1C = _BV(UMSEL) | _BV(UCSZ11) | _BV(UCSZ10);
  UCSR0C |= (0<<UMSEL00)|(0<<UMSEL01)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(0<<UCSZ02)|(1<<UCSZ01)|(1<<UCSZ00);    

  return &s_midi_device_usart0;
}

MidiDevice* serial_midi_init_usart1(uint16_t clockScale, bool out, bool in) {
  //send up the device
  midi_device_init(&s_midi_device_usart1);
  bytequeue_init(&queue_usart1, output_queue_data_usart1, MIDI_OUTPUT_QUEUE_LENGTH); 
  midi_device_set_send_func(&s_midi_device_usart1, serial_midi_send_usart1);

  // Set baud rate
  UBRR1H = (uint8_t)(clockScale >> 8);
  UBRR1L = (uint8_t)(clockScale & 0xFF);
  // Enable transmitter
  if(out)
    UCSR1B |= _BV(TXEN1);
  if(in) {
    //Enable receiver
    //RX Complete Interrupt Enable  (user must provide routine)
    UCSR1B |= _BV(RXEN1) | _BV(RXCIE1);
  }
  //Set frame format: Async, 8data, 1 stop bit, 1 start bit, no parity
  //needs to have URSEL set in order to write into this reg
  // UCSR1C = _BV(UMSEL) | _BV(UCSZ11) | _BV(UCSZ10);
  UCSR1C |= (0<<UMSEL10)|(0<<UMSEL11)|(0<<UPM11)|(0<<UPM10)|(0<<USBS1)|(0<<UCSZ12)|(1<<UCSZ11)|(1<<UCSZ10);    

  return &s_midi_device_usart1;
}

