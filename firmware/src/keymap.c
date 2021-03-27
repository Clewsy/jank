#include "keymap.h"


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
