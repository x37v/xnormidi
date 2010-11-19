/*
 * Modified by Alex Norman (x37v.info) from Dean Camera's MIDI LUFA demo
 * Original Copyright below
 */

/*
   LUFA Library
   Copyright (C) Dean Camera, 2010.

   dean [at] fourwalledcubicle [dot] com
   www.lufa-lib.org
   */

/*
   Copyright 2010  Dean Camera (dean [at] fourwalledcubicle [dot] com)

   Permission to use, copy, modify, distribute, and sell this
   software and its documentation for any purpose is hereby granted
   without fee, provided that the above copyright notice appear in
   all copies and that both that the copyright notice and this
   permission notice and warranty disclaimer appear in supporting
   documentation, and that the name of the author not be used in
   advertising or publicity pertaining to distribution of the
   software without specific, written prior permission.

   The author disclaim all warranties with regard to this
   software, including all implied warranties of merchantability
   and fitness.  In no event shall the author be liable for any
   special, indirect or consequential damages or any damages
   whatsoever resulting from loss of use, data or profits, whether
   in an action of contract, negligence or other tortious action,
   arising out of or in connection with the use or performance of
   this software.
   */

#include "midi_usb.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "Descriptors.h"

#include <LUFA/Version.h>
#include <LUFA/Drivers/USB/USB.h>

/* Function Prototypes: */
void SetupHardware(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);

//we disregard cnt because we assume all other bytes are zero and we always send 4 bytes
void usb_send_func(MidiDevice * device, uint8_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2) {
   /* Device must be connected and configured for the task to run */
   if (USB_DeviceState != DEVICE_STATE_Configured)
      return;

   Endpoint_SelectEndpoint(MIDI_STREAM_IN_EPNUM);

   while (!Endpoint_IsINReady());

   MIDI_EventPacket_t event;
   event.CableNumber = 0;
   event.Command = byte0 >> 4;
   event.Data1 = byte0;
   event.Data2 = byte1;
   event.Data3 = byte2;

   /* Write the MIDI event packet to the endpoint */
   Endpoint_Write_Stream_LE(&event, sizeof(event));

   /* Send the data in the endpoint to the host */
   Endpoint_ClearIN();
   USB_USBTask();
}

void usb_get_midi(MidiDevice * device) {
   /* Select the MIDI OUT stream */
   Endpoint_SelectEndpoint(MIDI_STREAM_OUT_EPNUM);

   /* Check if a MIDI command has been received */
   while (Endpoint_IsOUTReceived())
   {
      MIDI_EventPacket_t event;

      /* Read the MIDI event packet from the endpoint */
      Endpoint_Read_Stream_LE(&event, sizeof(event));

      midi_packet_length_t length = midi_packet_length(event.Data1);

      //pass the data to the device input function
      //not dealing with sysex yet
      if (length != UNDEFINED)
         midi_device_input(device, length, event.Data1, event.Data2, event.Data3);

      /* If the endpoint is now empty, clear the bank */
      if (!(Endpoint_BytesInEndpoint()))
      {
         /* Clear the endpoint ready for new packet */
         Endpoint_ClearOUT();
      }
   }
   USB_USBTask();
}

void midi_usb_init(MidiDevice * device){
   midi_device_init(device);
   midi_device_set_send_func(device, usb_send_func);
   midi_device_set_pre_input_process_func(device, usb_get_midi);

   SetupHardware();
   sei();
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
   /* Disable watchdog if enabled by bootloader/fuses */
   MCUSR &= ~(1 << WDRF);
   wdt_disable();

   /* Disable clock division */
   clock_prescale_set(clock_div_1);

   /* Hardware Initialization */
   USB_Init();
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs. */
void EVENT_USB_Device_Connect(void)
{
   //set some LED?
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs, disables the sample update and PWM output timers and stops the USB and MIDI management tasks.
 */
void EVENT_USB_Device_Disconnect(void)
{
   //set some LED?
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host set the current configuration
 *  of the USB device after enumeration - the device endpoints are configured and the MIDI management task started.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
   bool ConfigSuccess = true;

   /* Setup MIDI Data Endpoints */
   ConfigSuccess &= Endpoint_ConfigureEndpoint(MIDI_STREAM_IN_EPNUM, EP_TYPE_BULK, ENDPOINT_DIR_IN,
         MIDI_STREAM_EPSIZE, ENDPOINT_BANK_SINGLE);
   ConfigSuccess &= Endpoint_ConfigureEndpoint(MIDI_STREAM_OUT_EPNUM, EP_TYPE_BULK, ENDPOINT_DIR_OUT,
         MIDI_STREAM_EPSIZE, ENDPOINT_BANK_SINGLE);
   //set some LED?
}

