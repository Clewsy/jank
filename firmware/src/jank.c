#include "jank.h"

// This interrupt sub-routine is triggered by a counter configured to vary the PWM signal used to control the brightness of the
// LEDs.  When enabled this creates a pulsing effect with the LEDs.
ISR(PULSER_INT_VECTOR)
{
	leds_handle_pulser_interrupt();
}

// This interrupt sub-routine is trigerred when the dimmer/brightness button is pressed.  Pressing the button cycles the pwm duty
// cycle through various values, effectively stepping through various brigntness values of the LEDs.  Note the PWM signal controls a
// PNP transistor on the anode side of the LEDs, therefore duty cycle 255 = off.
ISR(BUTTON_PCI_VECTOR)
{
	// Button de-bounce.
	_delay_ms(BUTTON_DEBOUNCE_MS);

	// If the button is still pressed (i.e. not a bounce or a release), change the mode.
	if(leds_button_state()) leds_change_mode();

	// Keep the usb interface alive whilst waiting until the button is released.
	while(leds_button_state())
	{
		HID_Task();
		USB_USBTask();
	}
}

// Initialise the hardware peripherals.
void hardware_init(void)
{
	clock_prescale_set(clock_div_1);	// Ensure no pre-scaling (run full speed - 16MHz).
	leds_init();				// Defined in leds.c
	keyscan_init();				// Defined in keyscan.c
	SetupHIDHardware();			// Defined in Keyboard.c
}

// Main program entry point.
int main(void)
{
	hardware_init();
	GlobalInterruptEnable();

	while(true)
	{
		HID_Task();	// In Keyboard.c
		USB_USBTask();	// In the lufa library.
	}
}
