// This version of Keyboard.c has been heavily modified from the original demo in Dean Camera's LUFA repository.
// Effectively I have used combinations of my own functions in addition to functions from the following .c files:
// - Keyboard.c at https://github.com/abcminiuser/lufa/tree/790ac4d125c1c5d72016b52c2b9ad58f5e5f8c62/Demos/Device/LowLevel/Keyboard
// - KeyboardMouse.c at https://github.com/abcminiuser/lufa/tree/790ac4d125c1c5d72016b52c2b9ad58f5e5f8c62/Demos/Device/LowLevel/KeyboardMouse
// - MediaController.c at https://github.com/abcminiuser/lufa/tree/790ac4d125c1c5d72016b52c2b9ad58f5e5f8c62/Projects/MediaController
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

#include "Keyboard.h"

// Indicates what report mode the host has requested, true for normal HID reporting mode, false for special boot protocol reporting
// mode.
static bool UsingReportProtocol = true;

// Current Idle period. This is set by the host via a Set Idle HID class request to silence the device's reports for either the
// entire idle duration, or until the report status changes (e.g. the user presses a key).
static uint16_t IdleCount = 500;

// Current Idle period remaining. When the IdleCount value is set, tracks the remaining number of idle milliseconds. This is
// separate to the IdleCount timer and is incremented and compared as the host may request the current idle period via a Get Idle
// HID class request, thus its value must be preserved.
static uint16_t IdleMSRemaining = 0;

// Declaration of a keyscan_report_t structure that will be used to pass current keypresses to the keyboard reports and the media
// controller reports. 
static keyscan_report_t keyscan_report;

// Configures the board hardware and chip peripherals.
// Use case is specifically an ATmega32U4 (ARCH_AVR8).
void SetupHIDHardware(void)
{
	// Disable watchdog if enabled by bootloader/fuses.
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// Set numlock LED pin as an output.
	NUMLOCK_LED_DDR |= (1 << NUMLOCK_LED);

	// USB Initialization from LUFA library.
	USB_Init();
}

// Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and starts the library
// USB task to begin the enumeration and USB management process.
void EVENT_USB_Device_Connect(void)
{
	// Indicate USB enumerating with NUMLOCK LED.
	numlock_led(true);

	// Default to report protocol on connect.
	UsingReportProtocol = true;
}

// Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void)
{
	// Indicate USB not ready.
	numlock_led(false);
}

// Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration of the USB
// device after enumeration, and configures the keyboard device endpoints.
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	// Setup HID Report Endpoints.
	ConfigSuccess &= Endpoint_ConfigureEndpoint(KEYBOARD_IN_EPADDR, EP_TYPE_INTERRUPT, HID_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(KEYBOARD_OUT_EPADDR, EP_TYPE_INTERRUPT, HID_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(MEDIACONTROLLER_IN_EPADDR, EP_TYPE_INTERRUPT, HID_EPSIZE, 1);

	// Turn on Start-of-Frame events for tracking HID report period expiry.
	USB_Device_EnableSOFEvents();
}

// Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to the device
// from the USB host before passing along unhandled control requests to the library for processing internally.
void EVENT_USB_Device_ControlRequest(void)
{
	// Handle HID Class specific requests.
	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				// Determine if it is the keyboard or media controller data that is being requested.
				if (!(USB_ControlRequest.wIndex))
				{

					// Create the next keyboard report for transmission to the host.
					USB_KeyboardReport_Data_t KeyboardReportData;
					CreateKeyboardReport(&KeyboardReportData);
					// Write the report data to the control endpoint.
					Endpoint_Write_Control_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData));
				}
				else
				{
					// Create the next media controller report for transmission to the host.
					USB_MediaControllerReport_Data_t MediaControllerReportData;
					CreateMediaControllerReport(&MediaControllerReportData);
					// Write the report data to the control endpoint.
					Endpoint_Write_Control_Stream_LE(&MediaControllerReportData, sizeof(MediaControllerReportData));
				}
				Endpoint_ClearOUT();
				
				Endpoint_ClearSETUP();

			}
			break;

		case HID_REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				// Wait until the LED report has been sent by the host.
				while (!(Endpoint_IsOUTReceived()))
				{
					if (USB_DeviceState == DEVICE_STATE_Unattached)
					  return;
				}

				// Read in the LED report from the host.
				uint8_t LEDStatus = Endpoint_Read_8();

				Endpoint_ClearOUT();
				Endpoint_ClearStatusStage();

				// Process the incoming LED report.
				ProcessLEDReport(LEDStatus);
			}
			break;

		case HID_REQ_GetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				// Write the current protocol flag to the host.
				Endpoint_Write_8(UsingReportProtocol);

				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}
			break;

		case HID_REQ_SetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				// Set or clear the flag depending on what the host indicates is the current Protocol.
				UsingReportProtocol = (USB_ControlRequest.wValue != 0);
			}
			break;

		case HID_REQ_SetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				// Get idle period in MSB, IdleCount must be multiplied by 4 to get milliseconds.
				IdleCount = ((USB_ControlRequest.wValue & 0xFF00) >> 6);
			}
			break;

		case HID_REQ_GetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				// Write the current idle duration to the host.  Must be divided by 4 before send.
				Endpoint_Write_8(IdleCount >> 2);

				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}
			break;
	}
}

// Event handler for the USB device Start Of Frame event.
void EVENT_USB_Device_StartOfFrame(void)
{
	// One millisecond has elapsed, decrement the idle time remaining counter if it has not already elapsed.
	if (IdleMSRemaining) IdleMSRemaining--;
}

// Fills the given HID report data structure with the next keyboard HID input report to send to the host.
// ReportData: Pointer to a HID report data structure to be filled.
// Report data derived from keyscan report created by the create_keyscan_report() function in keyscan.c.
void CreateKeyboardReport(USB_KeyboardReport_Data_t* const ReportData)
{
	// Clear the report contents.
	memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

	// Update the modifier byte from the last keyscan report.
	ReportData->Modifier = keyscan_report.modifier;

	// Update the keys from the last keyscan report.
	for(uint8_t i = 0; keyscan_report.keys[i] != 0x00; i++)  ReportData->KeyCode[i] = keyscan_report.keys[i];
}

// Fills the given HID report data structure with the next media controller HID input report to send to the host.
// MediaReportData:  Pointer to a HID report data structure to be filled.
void CreateMediaControllerReport(USB_MediaControllerReport_Data_t* const MediaReportData)
{
	// Clear the report contents.
	memset(MediaReportData, 0, sizeof(USB_MediaControllerReport_Data_t));

	// Update the Media Control report flags from the latest keyscan report.
	MediaReportData->Play		= (keyscan_report.media_keys & (1 << MK_PLAY)		? true : false);
	MediaReportData->Pause		= (keyscan_report.media_keys & (1 << MK_PAUSE)		? true : false);
	MediaReportData->FForward	= (keyscan_report.media_keys & (1 << MK_FF)		? true : false);
	MediaReportData->Rewind		= (keyscan_report.media_keys & (1 << MK_RW)		? true : false);
	MediaReportData->NextTrack	= (keyscan_report.media_keys & (1 << MK_NEXT)		? true : false);
	MediaReportData->PreviousTrack	= (keyscan_report.media_keys & (1 << MK_PREVIOUS)	? true : false);
	MediaReportData->Stop		= (keyscan_report.media_keys & (1 << MK_STOP)		? true : false);
	MediaReportData->PlayPause	= (keyscan_report.media_keys & (1 << MK_TOGGLE)		? true : false);
	MediaReportData->Mute		= (keyscan_report.media_keys & (1 << MK_MUTE)		? true : false);
	MediaReportData->VolumeUp	= (keyscan_report.media_keys & (1 << MK_VOL_UP)		? true : false);
	MediaReportData->VolumeDown	= (keyscan_report.media_keys & (1 << MK_VOL_DOWN)	? true : false);
}

// Similar to CreateKeyboardReport() function but creates a report for a single specific keypress.
void CreateMacroKeyReport(USB_KeyboardReport_Data_t* const ReportData, uint8_t keys[MAX_KEYS], uint8_t modifier)
{
	// Clear the report contents.
	memset(ReportData, 0, sizeof(USB_KeyboardReport_Data_t));

	// Apply left shift key if desired character is upper-case.
	ReportData->Modifier = modifier;

	// Apply the character key code.
	for(uint8_t k = 0; k < MAX_KEYS; k++) ReportData->KeyCode[k] = keys[k];
}

// Set or clear the smd LED on the bottom of the board used to indicate the status of Num Lock.
void numlock_led(bool on)
{
	if (on)	NUMLOCK_LED_PORT |= (1 << NUMLOCK_LED);
	else 	NUMLOCK_LED_PORT &= ~(1 << NUMLOCK_LED);
}

// Processes a received LED report, and updates the board LEDs states to match.
// LEDReport: LED status report from the host.
void ProcessLEDReport(const uint8_t LEDReport)
{
	if (LEDReport & HID_KEYBOARD_LED_NUMLOCK)	numlock_led(true);
	else 						numlock_led(false);
}

// Sends the next keyboard HID report to the host, via the keyboard data endpoint.
void SendNextKeyboardReport(void)
{
	static USB_KeyboardReport_Data_t PrevKeyboardReportData;
	USB_KeyboardReport_Data_t        KeyboardReportData;
	bool                             SendReport = false;

	// Create the next keyboard report for transmission to the host.
	CreateKeyboardReport(&KeyboardReportData);

	// Check if the idle period is set and has elapsed.
	if (IdleCount && (!(IdleMSRemaining)))
	{
		// Reset the idle time remaining counter.
		IdleMSRemaining = IdleCount;

		// Idle period is set and has elapsed, must send a report to the host.
		SendReport = true;
	}
	else
	{
		// Check to see if the report data has changed - if so a report MUST be sent.
		SendReport = (memcmp(&PrevKeyboardReportData, &KeyboardReportData, sizeof(USB_KeyboardReport_Data_t)) != 0);
	}

	// Select the Keyboard Report Endpoint.
	Endpoint_SelectEndpoint(KEYBOARD_IN_EPADDR);

	// Check if Keyboard Endpoint Ready for Read/Write and if we should send a new report.
	if (Endpoint_IsReadWriteAllowed() && SendReport)
	{
		// Save the current report data for later comparison to check for changes.
		PrevKeyboardReportData = KeyboardReportData;

		// Write Keyboard Report Data.
		Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData), NULL);

		// Finalize the stream transfer to send the last packet.
		Endpoint_ClearIN();
	}

	// Dirty delay to prevent button bounce registering as a double-press.
	_delay_ms(DEBOUNCE_MS);
}

// Sends the next media controller HID report to the host, via the keyboard data endpoint.
// This function is very similar to the keyboard equivalent but was created for media controller reports.
void SendNextMediaControllerReport(void)
{
	static USB_MediaControllerReport_Data_t	PrevMediaControllerReportData;
	USB_MediaControllerReport_Data_t	MediaControllerReportData;
	bool					SendReport = false;

	// Create the next keyboard report for transmission to the host.
	CreateMediaControllerReport(&MediaControllerReportData);

	// Check if the idle period is set and has elapsed.
	if (IdleCount && (!(IdleMSRemaining)))
	{
		// Reset the idle time remaining counter.
		IdleMSRemaining = IdleCount;

		// Idle period is set and has elapsed, must send a report to the host.
		SendReport = true;
	}
	else
	{
		// Check to see if the report data has changed - if so a report MUST be sent.
		SendReport = (memcmp(&PrevMediaControllerReportData, &MediaControllerReportData, sizeof(USB_MediaControllerReport_Data_t)) != 0);
	}

	// Select the Keyboard Report Endpoint.
	Endpoint_SelectEndpoint(MEDIACONTROLLER_IN_EPADDR);

	// Check if Keyboard Endpoint Ready for Read/Write and if we should send a new report.
	if (Endpoint_IsReadWriteAllowed() && SendReport)
	{
		// Save the current report data for later comparison to check for changes.
		PrevMediaControllerReportData = MediaControllerReportData;

		// Write Keyboard Report Data */
		Endpoint_Write_Stream_LE(&MediaControllerReportData, sizeof(MediaControllerReportData), NULL);

		// Finalize the stream transfer to send the last packet.
		Endpoint_ClearIN();
	}
}

// Reads the next LED status report from the host from the LED data endpoint, if one has been sent.
void ReceiveNextKeyboardReport(void)
{
	// Select the Keyboard LED Report Endpoint.
	Endpoint_SelectEndpoint(KEYBOARD_OUT_EPADDR);

	// Check if Keyboard LED Endpoint contains a packet.
	if (Endpoint_IsOUTReceived())
	{
		// Check to see if the packet contains data.
		if (Endpoint_IsReadWriteAllowed())
		{
			// Read in the LED report from the host.
			uint8_t LEDReport = Endpoint_Read_8();

			// Process the read LED report from the host.
			ProcessLEDReport(LEDReport);
		}

		// Handshake the OUT Endpoint - clear endpoint and ready for next report.
		Endpoint_ClearOUT();
	}
}

void SendMacroReports()
{
	// Read in the address of a macro that corresponds to an actuated macro keyswitch.
	const macro_t *macro = scan_macro_keys();

	// m_value is used to increment for each macro action in the macro array.
	uint8_t m_count = 0;

	// If the value returned by scan_macro_keys is not null.
	if (macro)
	{
		// Loop for every valid "macro action".  Type macro_t represents a single such action and "macro" is actually the
		// address of the first macro_t elements in an array.  This while loop will run until a null value is read which
		// indicates the last element in the macro array.
		uint8_t current_macro_action = pgm_read_byte(&macro->m_action + (m_count * sizeof(macro_t)));
		while(current_macro_action)
		{
			// The m_action element of a macro_t struct identifies the succedding array (m_array) as either a string to
			// be typed sequentially, a combination of keys to be pressed simultaneously, a media keypress or a delay.

			switch(current_macro_action)
			{
				// If the macro action type is a string.
				case M_STRING: ;

					// Loop for each character in the macro array.
					uint8_t c = 0;
					while((pgm_read_byte(&macro->m_array[c] + (m_count * sizeof(macro_t)))) && (c < MAX_MACRO_CHARS))
					{
						type_key(pgm_read_byte(&macro->m_array[c++] + (m_count * sizeof(macro_t))));
						USB_USBTask();	// Keep the USB device alive (in case of long strings).
					}

					break;
	
				// If the macro action type is a combination of keys.
				case M_KEYS: ;

					// Initialise an array of keys and a byte that represents the state of all modifiers.
					uint8_t macro_keys[MAX_KEYS] = {0};
					uint8_t macro_modifiers = 0;

					// Loop for each key identified in the macro array.
					uint8_t k = 0;
					while((pgm_read_byte(&macro->m_array[k] + (m_count * sizeof(macro_t)))) && (k < MAX_MACRO_CHARS))
					{
						uint8_t current_key = (pgm_read_byte(&macro->m_array[k++] + (m_count * sizeof(macro_t))));

						// Regular keys scan values range from 0x00 to 0x65.
						if(current_key <= HID_KEYBOARD_SC_APPLICATION)
						{
							// Skip array elements that already have a keyscan value written.
							uint8_t j = 0;
							while((macro_keys[j]) && (j < MAX_KEYS)) j++;
					
							// Only register the key if the max simultaneous keys is not reached.
							if(j < MAX_KEYS) macro_keys[j] = current_key;
						}

						// Modifier keys scan values start at 0xE0 (after  last keyboard modifier key scan).
						else if(current_key <= HID_KEYBOARD_SC_RIGHT_GUI)
						{
							// Convert the media key to a value from 0 to 7.
							current_key -= HID_KEYBOARD_SC_LEFT_CONTROL;
							// Shift a bit to the corresponding bit within the modifier integer.
							macro_modifiers |= (1 << current_key);
						}
					}

					// We now know the combination of keys and modifiers, so send the report.
					SendNextMacroKeyReport(macro_keys, macro_modifiers);

					// Dirty delay to prevent button bounce registering as a double-press.
					_delay_ms(DEBOUNCE_MS);

					// Send a "no-key" (i.e. release the key/s).
					SendNextMacroKeyReport(0x00, 0x00);

					break;

				// If the macro action type is a delay/pause.
				case M_WAIT: ;

					// Loop through each element of the macro array.
					uint8_t d = 0;
					while((pgm_read_byte(&macro->m_array[d] + (m_count * sizeof(macro_t)))) && (d < MAX_MACRO_CHARS))
					{
						// Each element of the array represents a number of seconds to pause.
						for(uint8_t seconds = (pgm_read_byte(&macro->m_array[d++] + (m_count * sizeof(macro_t))));seconds > 0; seconds--)
						{
							// Each second-long delay is broken into 4 quarter-seconds.
							for(uint8_t quarters = 0; quarters < 4; quarters++)
							{
								_delay_ms(250);	// Quarter-second delay.
								USB_USBTask();	// Keep the USB device alive.
							}
						}
					}

					break;
			}

			// Increment the macro action counter.
			m_count++;

			// Update the current macro to the next macro_t in memory.
			current_macro_action = pgm_read_byte(&macro->m_action + (m_count * sizeof(macro_t)));
		}

		// Wait until the key is released (if held) while keeping the USB device alive.
		while(scan_macro_keys()) USB_USBTask();
		// Send a "no-key" (i.e. release the key/s).
		SendNextMacroKeyReport(0x00, 0x00);
	}
}

// Effectivley send a keyboard report but for a single character. 
void type_key(char key)
{
	uint8_t single_key[MAX_KEYS] = {char_to_code(key)};
	SendNextMacroKeyReport(single_key, (upper_case_check(key) << 1));
	SendNextMacroKeyReport(0x00, 0x00);	// Send a "no-key" after each actual char (i.e. release the key).
}

// Similar to the SendNextKeyboardReport() function, but types a single key (with or without modifiers). Intended to be used
// sequentially to "type" a string of characters - i.e. a macro.
void SendNextMacroKeyReport(uint8_t keys[MAX_KEYS], uint8_t modifiers)
{
	USB_KeyboardReport_Data_t        MacroReportData;

	// Create the next keyboard report for transmission to the host.
	CreateMacroKeyReport(&MacroReportData, keys, modifiers);

	// Select the Keyboard Report Endpoint.
	Endpoint_SelectEndpoint(KEYBOARD_IN_EPADDR);

	// Wait until Keyboard Endpoint Ready for Read/Write.
	while(!Endpoint_IsReadWriteAllowed()) {}

	// Write Keyboard Report Data.
	Endpoint_Write_Stream_LE(&MacroReportData, sizeof(MacroReportData), NULL);

	// Finalize the stream transfer to send the last packet.
	Endpoint_ClearIN();
}

// Function to manage HID report generation and transmission to the host, when in report mode.
void HID_Task(void)
{
	// Device must be connected and configured for the task to run.
	if (USB_DeviceState != DEVICE_STATE_Configured) return;

	// Check for and action any key presses designated as macros.  Only if at least one macro is defined.
	if(pgm_read_byte(&MACROMAP[0][0][0]))	SendMacroReports();

	// Update the keyscan report - will be used for creating both the keyboard and media controller reports.
	create_keyscan_report(&keyscan_report);

	// Send the next keypress report to the host.
	SendNextKeyboardReport();

	// Process the LED report sent from the host.
	ReceiveNextKeyboardReport();

	// Send the next media controller keypress report to the host.
	SendNextMediaControllerReport();
}
