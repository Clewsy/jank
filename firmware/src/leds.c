#include "leds.h"

// Each "mode" is defined by a uint8_t for brightness and a uint16_t for pulse speed.
// initial_brightness:	The PWM value to send to the LEDs before setting the pulse speed.  Note, the LEDs are active low, so full
//			brightness requires a value of 0xFF.  Range is 0 to 255 (0x00 to 0xFF).
// pulse_speed:		When the pulse timer counter reaches this value, an interrupt is triggered that changes the PWM value.
//			Set to zero for no pulse effect.  Range is 0 to 65535 (0x0000 to 0xFFFF).
const struct mode modes[] = 
{
	{ .initial_brightness = 0, 	.pulse_speed = 0},	// Mode 00 - Off.
	{ .initial_brightness = 2, 	.pulse_speed = 0},	// Mode 01
	{ .initial_brightness = 4, 	.pulse_speed = 0},	// Mode 02
	{ .initial_brightness = 8, 	.pulse_speed = 0},	// Mode 03
	{ .initial_brightness = 16, 	.pulse_speed = 0},	// Mode 04
	{ .initial_brightness = 32, 	.pulse_speed = 0},	// Mode 05
	{ .initial_brightness = 64, 	.pulse_speed = 0},	// Mode 06
	{ .initial_brightness = 128, 	.pulse_speed = 0},	// Mode 07
	{ .initial_brightness = 255, 	.pulse_speed = 0},	// Mode 08 - Max brightness.
	{ .initial_brightness = 255, 	.pulse_speed = 2000},	// Mode 09 - Fast pulse.
	{ .initial_brightness = 255, 	.pulse_speed = 8000},	// Mode 10 - Medium pulse.
	{ .initial_brightness = 255, 	.pulse_speed = 32000},	// Mode 11 - Slow pulse.

};

// Initialise the AVR registers for controlling the LEDs.
// The hardware configuration has the pwm pin connected to a PNP transistor that controls all LEDs on the anode side.
void leds_init(void)
{
	////////Initialise the button interrupt.
	BUTTON_DDR &= ~(1 << BUTTON_PIN);	// Set button pin as input.
	BUTTON_PORT |= (1 << BUTTON_PIN);	// Enable internal pull-ups.
	BUTTON_PCMSK |= (1 << BUTTON_PCINT);	// Enable pin-change interrupt for selected pin.

	////////Initialise the pwm timer.
	PWM_DDR |= (1 << PWM_PIN);	// Set the PWM pin as an output.
	// WGM[2:0] set to 111	: Fast PWM mode with TOP value set by output compare register.
	// COM[1:0] set to 10	: Clear on compare match, set output pin at TOP.
	// CS[2:0] set to 011	: clk = CK/64 = 250,000Hz.  PWM_freq = clk/256 = 964.5625Hz
	PWM_TCCRA |= ((1 << PWM_COM1) | (1 << PWM_WGM1) | (1 << PWM_WGM0));
	PWM_TCCRB |= ((0 << PWM_WGM2) | (1 << PWM_CS1) | (1 << PWM_CS0));

	////////Initialise the pulse effect timer.
	// WGM[2:0] set to 0100 : CTC mode, counts from 0 to value of output compare register.
	// COM[1:0] set to 00 : Normal pin modes - not connected to timer.
	// CS[2:0] set to 010 : clk/8 (from prescaler).
	PULSER_TCCRB |= ((1 << PULSER_WGM2) | (1 << PULSER_CS1));

	// Set the default (start-up) led mode.
	leds_set_mode(START_MODE);

	// Enable the led mode button.
	leds_button_enable(true);
}

// Set the current duty cycle (0 to 255).
void leds_pwm_set(uint8_t duty)
{
	PWM_SET = duty;
}

// Get the current duty cycle (0 to 255).
uint8_t leds_pwm_get(void)
{
	return(PWM_SET);
}

// Enable or disable the pulse effect.
void leds_pulse_enable(bool enable)
{
	if(enable)	PULSER_TIMSK |= (1 << PULSER_IE);	// Enable the output compare interrupt.
	else		PULSER_TIMSK &= ~(1 << PULSER_IE);	// Disable the output compare interrupt.
}

// Set the counter value that will trigger the interrupt.  Also disable the interrupt if the selected speed is zero.
void leds_pulse_speed_set(uint16_t speed)
{
	PULSER_SET_REG = speed;

	if(speed)	leds_pulse_enable(true);
	else		leds_pulse_enable(false);	// Enable pulse only if speed is not zero.
}

// Using the look-up table ("modes"), set the desired brightness (pwm value) and pulse speed.
void leds_set_mode(uint8_t led_mode)
{
	leds_pwm_set(modes[led_mode].initial_brightness);
	leds_pulse_speed_set(modes[led_mode].pulse_speed);
}

// Update the pwm value when the pulse effect interrupt is triggered.
void leds_handle_pulser_interrupt(void)
{
	static int8_t led_pulse_direction = 1;	// Static so it is initiaised once but retained for future invocations.
	uint8_t current_pwm = leds_pwm_get();	// Check the current pwm value.

	if	(current_pwm == 255)	led_pulse_direction = -1;	// Start decreasing.
	else if	(current_pwm == 0)	led_pulse_direction = 1;	// Start increasing.

	leds_pwm_set(current_pwm + led_pulse_direction);		// Update the led brightness.
}

// Cycle through the various led modes.
void leds_change_mode(void)
{
	static uint8_t led_mode = START_MODE;	// Static so it is initiaised once but retained for future invocations.

	if(led_mode == (NUM_MODES - 1))	led_mode = 0;	// Cycle back to the first mode if last mode reached.
	else				led_mode++;	// Else just increment to the next mode.

	leds_set_mode(led_mode);
}

// Enable pr disable the mode button.
void leds_button_enable(bool enable)
{
	if(enable)	BUTTON_PCICR |= (1 << BUTTON_PCIE);	// Enable the button pin-change interrupt.
	else		BUTTON_PCICR &= ~(1 << BUTTON_PCIE);	// Disable the button pin-change interrupt.
}

// Check the state of the pin.  Returns true if pressed.
bool leds_button_state(void)
{
	if(~BUTTON_PINS & (1 << BUTTON_PIN))	return(true);
	else					return(false);
}
