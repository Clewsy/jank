#include "keymap.h"


// The key map array - for regular key strokes including media control keys.
const char KEYMAP[NUM_ROWS][NUM_COLS] PROGMEM = {
	// Column 0                     	  	Column 1					Column 2					Column 3
	{HID_KEYBOARD_SC_A,				HID_MEDIACONTROLLER_SC_VOLUME_UP,		HID_MEDIACONTROLLER_SC_MUTE,			HID_MEDIACONTROLLER_SC_VOLUME_DOWN},	// Row 0
	{HID_KEYBOARD_SC_NUM_LOCK,			HID_KEYBOARD_SC_KEYPAD_SLASH,			HID_KEYBOARD_SC_KEYPAD_ASTERISK,		HID_KEYBOARD_SC_KEYPAD_MINUS},		// Row 1
	{HID_KEYBOARD_SC_KEYPAD_7_AND_HOME,		HID_KEYBOARD_SC_KEYPAD_8_AND_UP_ARROW,		HID_KEYBOARD_SC_KEYPAD_9_AND_PAGE_UP,		HID_KEYBOARD_SC_KEYPAD_PLUS},		// Row 2
	{HID_KEYBOARD_SC_KEYPAD_4_AND_LEFT_ARROW,	HID_KEYBOARD_SC_KEYPAD_5,			HID_KEYBOARD_SC_KEYPAD_6_AND_RIGHT_ARROW,	0x00},					// Row 3
	{HID_KEYBOARD_SC_KEYPAD_1_AND_END,		HID_KEYBOARD_SC_KEYPAD_2_AND_DOWN_ARROW,	HID_KEYBOARD_SC_KEYPAD_3_AND_PAGE_DOWN,		HID_KEYBOARD_SC_KEYPAD_ENTER},		// Row 4
	{HID_KEYBOARD_SC_KEYPAD_0_AND_INSERT,		0x00,						HID_KEYBOARD_SC_KEYPAD_DOT_AND_DELETE,		0x00}					// Row 5
};



/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Example configuration: All four keys are media controls, toggle, stop, previous & next.
const uint8_t KEY_PIN_ARRAY[] = {KEY_1, KEY_2, KEY_3, KEY_4};
const uint8_t MACRO_PIN_ARRAY[] = {};

const char KEY_MAP[] PROGMEM = {
	HID_MEDIACONTROLLER_SC_TOGGLE,		// Key 1
	HID_MEDIACONTROLLER_SC_STOP,		// Key 2
	HID_MEDIACONTROLLER_SC_PREVIOUS,	// Key 3
	HID_MEDIACONTROLLER_SC_NEXT		// Key 4
};

const char MACRO_MAP[][MAX_MACRO_CHARS] PROGMEM = {};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Example configuration: Top two keys are media controls (toggle & stop), bottom two keys are macros.
const uint8_t KEY_PIN_ARRAY[] = {KEY_1, KEY_2};
const uint8_t MACRO_PIN_ARRAY[] = {KEY_3, KEY_4};

const char KEY_MAP[] PROGMEM = {
	HID_MEDIACONTROLLER_SC_TOGGLE,		// Key 1
	HID_MEDIACONTROLLER_SC_STOP,		// Key 2
};

const char MACRO_MAP[][MAX_MACRO_CHARS] PROGMEM = {
	{"      _\n     ( )\n      H\n      H\n     _H_\n  .-'-.-'-.\n /         \\\n|           |\n|   .-------'._\n|  / /  '.' '. \\\n|  \\ \\ @   @ / /\n|   '---------'\n|    _______|\n|  .'-+-+-+|\n|  '.-+-+-+|\n|    \"\"\"\"\"\" |\n'-.__   __.-'\n     \"\"\"\n\n"},				// Key 3.
	{"        _\n      /X \\ \n    _------_\n   /        \\\n  |          |\n  |          |\n  |     __  __)\n  |    /  \\/  \\\n /\\/\\ (o   )o  )\n /c    \\__/ --.\n \\_   _-------'\n  |  /         \\\n  | | '\\________)\n  |  \\_____)\n  |_____ |\n |_____/\\/\\\n /        \\\n\n"},	// Key 4.
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Example configuration: Keys are equivalent to Left-Shift, 'a', 'b' & 'c'.
const uint8_t KEY_PIN_ARRAY[] = {KEY_1, KEY_2, KEY_3, KEY_4};
const uint8_t MACRO_PIN_ARRAY[] = {};

const char KEY_MAP[] PROGMEM = {
	HID_KEYBOARD_SC_LEFT_SHIFT,	// Key 1
	HID_KEYBOARD_SC_A,		// Key 2
	HID_KEYBOARD_SC_B,		// Key 3
	HID_KEYBOARD_SC_C		// Key 4
};

const char MACRO_MAP[][MAX_MACRO_CHARS] PROGMEM = {};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
