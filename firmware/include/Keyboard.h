// This version of Keyboard.h has been modified from the original demo in Dean Camera's LUFA repository.
// These changes are similarly MIT licensed (refer license at https://gitlab.com/clewsy/jank).

/*
             LUFA Library
     Copyright (C) Dean Camera, 2020.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2020  Dean Camera (dean [at] fourwalledcubicle [dot] com)
  Copyright 2010  Denver Gingerich (denver [at] ossguy [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/


#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

	// Includes:
	#include <avr/io.h>
	#include <avr/wdt.h>
	#include <avr/power.h>
	#include <avr/interrupt.h>
	#include <stdbool.h>
	#include <string.h>

	#include "Descriptors.h"

	#include <LUFA/Drivers/USB/USB.h>
	#include <LUFA/Drivers/Board/Joystick.h>
	#include <LUFA/Drivers/Board/Buttons.h>
	#include <LUFA/Drivers/Board/LEDs.h>
	#include <LUFA/Platform/Platform.h>

	// keyscan.h and .c files written for specific use-case, custom board.
	#include "keyscan.h"

	// Needed to prevent double key-presses.  Small number of keys means key-scanning happens a bit too quickly.
	#define DEBOUNCE_MS	1

	// Definitions needed for controlling the LED to indicate numlock status.
	#define NUMLOCK_LED_PORT	PORTB
	#define NUMLOCK_LED_DDR		DDRB
	#define NUMLOCK_LED		PB6


	// Type Defines:
	// Type define for a Media Control HID report. This report contains the bits to match the usages defined in the HID report
	// of the device.  When set to a true, the relevant media controls on the host will be triggered.
	typedef struct
	{
		unsigned Play           : 1;
		unsigned Pause          : 1;
		unsigned FForward       : 1;
		unsigned Rewind         : 1;
		unsigned NextTrack      : 1;
		unsigned PreviousTrack  : 1;
		unsigned Stop           : 1;
		unsigned PlayPause      : 1;
		unsigned Mute           : 1;
		unsigned VolumeUp       : 1;
		unsigned VolumeDown     : 1;
		unsigned RESERVED       : 5;
	} ATTR_PACKED USB_MediaControllerReport_Data_t;

	// The following struct for keyboard reports is defined in the lufa library HIDClassCommon.h file.  It's
	// included here for convenient reference.
//	typedef struct
//	{
//		uint8_t Modifier; /**< Keyboard modifier byte, indicating pressed modifier keys (a combination of
//		                   *   \c HID_KEYBOARD_MODIFER_* masks).
//		                   */
//		uint8_t Reserved; /**< Reserved for OEM use, always set to 0. */
//		uint8_t KeyCode[6]; /**< Key codes of the currently pressed keys. */
//	} ATTR_PACKED USB_KeyboardReport_Data_t;

	// Function Prototypes:
	void SetupHIDHardware(void);
	void HID_Task(void);

	void EVENT_USB_Device_Connect(void);
	void EVENT_USB_Device_Disconnect(void);
	void EVENT_USB_Device_ConfigurationChanged(void);
	void EVENT_USB_Device_ControlRequest(void);
	void EVENT_USB_Device_StartOfFrame(void);
	void CreateKeyboardReport(USB_KeyboardReport_Data_t* const ReportData);
	void CreateMediaControllerReport(USB_MediaControllerReport_Data_t* const MediaReportData);
	void CreateMacroKeyReport(USB_KeyboardReport_Data_t* const ReportData, char key_code, bool upper_case);
	void numlock_led(bool on);
	void ProcessLEDReport(const uint8_t LEDReport);
	void SendNextKeyboardReport(void);
	void ReceiveNextKeyboardReport(void);
	void SendNextMediaControllerReport(void);
	void SendMacroReports(const char *macro_string);
	void type_key(char key);
	void SendNextMacroKeyReport(uint8_t key_code, bool upper_case);

#endif
