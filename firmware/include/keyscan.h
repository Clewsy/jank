#include <avr/io.h>
#include <string.h>		// Included for memset function.
#include <stdbool.h>		// Included to use bool type and true/false values.
#include "keymap.h"




#define ROWS_PORT	PORTD
#define ROWS_DDR	DDRD
#define ROWS_PINS	PIND
#define COLS_PORT	PORTF
#define COLS_DDR	DDRF
#define COLS_PINS	PINF



#define ROW0	PD0
#define ROW1	PD1
#define ROW2	PD2
#define ROW3	PD3
#define ROW4	PD4
#define ROW5	PD5
#define COL0	PF0
#define COL1	PF1
#define COL2	PF4
#define COL3	PF5
// !IMPORTANT! - By default, the JTAGEN (JTAG enable) bit is set (i.e. actually value zero which means enabled in the land of AVR
// fuse bytes).  It must be cleared (i.e. set to 1!) otherwise PF4 and PF5 cannot be used as GPIO.  Out of the box the AtMega32U4
// low fuse (lfuse) byte was set to 0x99.  Writing  this to 0xD9 disabled JTAG. 



//#define KEYS_PORT	PORTD	// Hardware Port definition for setting pull-ups.
//#define KEYS_PINS	PIND	// Hardware Pins definition for reading keys.
//#define KEYS_DDR	DDRD	// Hardware data direction register.
#define MAX_KEYS	6	// Max number of simultaneous key-presses (excluding media keys and modifiers).

// Bit-shift definitions for the uint16_t media_keys integer:
#define MK_PLAY		 0
#define MK_PAUSE	 1
#define MK_FF		 2
#define MK_RW		 3
#define MK_NEXT		 4
#define MK_PREVIOUS	 5
#define MK_STOP		 6
#define MK_TOGGLE	 7
#define MK_MUTE		 8
#define MK_VOL_UP	 9
#define MK_VOL_DOWN	10

// Type define for a keyscan report which contains data identifying any current keypresses.
// See comments below for a breakdown of this struct.
typedef struct
{
	uint16_t media_keys;
	uint8_t modifier;
	uint8_t keys[MAX_KEYS];
} keyscan_report_t;

// Function declarations.
void keyscan_init(void);
void handle_key(char key, keyscan_report_t *keyscan_report);
void create_keyscan_report(keyscan_report_t *keyscan_report);
const char *scan_macro_keys(void);
uint8_t char_to_code(char key);
bool upper_case_check(char key);


/* Breakdown of a keyscan_report:

modifier: 8 bits each represent the state of a modifier:
lsb	bit0	Left Control
	bit1	Left Shift
	bit2	Left Alt
	bit3	Left Gui
	bit4	Right Control
	bit5	Right Shift
	bit6	Right Alt
msb	bit7	Right Gui

keys[6]: each key is represented by a byte 0x00 to 0x65.
A maximum of six simultaneous key presses will be registered.

media_keys: 11 bits each represent the state of a media key:
lsb	bit00	Play 
	bit01	Pause 
	bit02	Fast Forward 
	bit03	Rewind 
	bit04	Next Track 
	bit05	Previous Track 
	bit06	Stop 
	bit07	Play/Pause (toggle) 
	bit08	Mute 
	bit09	Volume Up 
	bit10	Volume Down 
	bit11	n/a (ignored)
	bit12	n/a (ignored)
	bit13	n/a (ignored)
	bit14	n/a (ignored)
msb	bit15	n/a (ignored)
*/
