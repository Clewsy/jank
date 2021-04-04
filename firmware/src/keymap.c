#include "keymap.h"

// Define the physical row and column pins on the microcontroller to be scanned for key presses.
const uint8_t key_row_array[] = {ROW1, ROW2, ROW3, ROW4, ROW5};
const uint8_t key_col_array[] = {COL0, COL1, COL2, COL3};
const uint8_t macro_row_array[] = {ROW0};
const uint8_t macro_col_array[] = {COL0, COL1, COL2, COL3};


// The key map array - for regular key strokes including media control keys.
const char KEYMAP[NUM_KEY_ROWS][NUM_KEY_COLS] PROGMEM = {
	// Column 0					Column 1					Column 2					Column 3
	{HID_KEYBOARD_SC_NUM_LOCK,			HID_KEYBOARD_SC_KEYPAD_SLASH,			HID_KEYBOARD_SC_KEYPAD_ASTERISK,		HID_KEYBOARD_SC_KEYPAD_MINUS},		// Row 1
	{HID_KEYBOARD_SC_KEYPAD_7_AND_HOME,		HID_KEYBOARD_SC_KEYPAD_8_AND_UP_ARROW,		HID_KEYBOARD_SC_KEYPAD_9_AND_PAGE_UP,		HID_KEYBOARD_SC_KEYPAD_PLUS},		// Row 2
	{HID_KEYBOARD_SC_KEYPAD_4_AND_LEFT_ARROW,	HID_KEYBOARD_SC_KEYPAD_5,			HID_KEYBOARD_SC_KEYPAD_6_AND_RIGHT_ARROW,	0x00},					// Row 3
	{HID_KEYBOARD_SC_KEYPAD_1_AND_END,		HID_KEYBOARD_SC_KEYPAD_2_AND_DOWN_ARROW,	HID_KEYBOARD_SC_KEYPAD_3_AND_PAGE_DOWN,		HID_KEYBOARD_SC_KEYPAD_ENTER},		// Row 4
	{HID_KEYBOARD_SC_KEYPAD_0_AND_INSERT,		0x00,						HID_KEYBOARD_SC_KEYPAD_DOT_AND_DELETE,		0x00}					// Row 5
};


// The macro map array - for key strokes that are mapped as macros.
const macro_t MACROMAP[NUM_MACRO_ROWS][NUM_MACRO_COLS][MAX_MACRO_ACTIONS] PROGMEM = 
{
	{ //Row0
		{ //Col0
			{M_KEYS, {HID_KEYBOARD_SC_LEFT_GUI, HID_KEYBOARD_SC_T}},
		},
		{ //Col1
			{M_KEYS, {HID_KEYBOARD_SC_LEFT_GUI}},
			{M_WAIT, {1}},
			{M_STRING, "firefox" },
			{M_KEYS, {HID_KEYBOARD_SC_ENTER}},
			{M_WAIT, {1}},
			{M_KEYS, {HID_KEYBOARD_SC_LEFT_CONTROL, HID_KEYBOARD_SC_T}},
			{M_STRING, "https://clews.pro/projects/jank.php" },
			{M_KEYS, {HID_KEYBOARD_SC_ENTER}},
		},
		{ //Col2
			{M_STRING, "      _\n"},
			{M_STRING, "     ( )\n"},
			{M_STRING, "      H\n"},
			{M_STRING, "      H\n"},
			{M_STRING, "     _H_\n"},
			{M_STRING, "  .-'-.-'-.\n"},
			{M_STRING, " /         \\\n"},
			{M_STRING, "|           |\n"},
			{M_STRING, "|   .-------'._\n"},
			{M_STRING, "|  / /  '.' '. \\\n"},
			{M_STRING, "|  \\ \\ @   @ / /\n"},
			{M_STRING, "|   '---------'\n"},
			{M_STRING, "|    _______|\n"},
			{M_STRING, "|  .'-+-+-+|\n"},
			{M_STRING, "|  '.-+-+-+|\n"},
			{M_STRING, "|    '''''' |\n"},
			{M_STRING, "'-.__   __.-'\n"},
			{M_STRING, "     '''\n"}
		},
		{ //Col3
			{M_STRING, "Bender is great!" },
			{M_KEYS, {HID_KEYBOARD_SC_ENTER}},
		}
	}
};

/*
Row and column configuration of the jank keypad.

	 COL0 COL1 COL2 COL3
	_____________________
	|    |    |    |    |
ROW0	| 00 | 01 | 02 | 03 |
	|____|____|____|____|
	_____________________
	|    |    |    |    |
ROW1	| 10 | 11 | 12 | 13 |
	|____|____|____|____|
	|    |    |    |    |
ROW2	| 20 | 21 | 22 | 23 |
	|____|____|____|    |
	|    |    |    |    |
ROW3	| 30 | 31 | 32 |    |
	|____|____|____|____|
	|    |    |    |    |
ROW4	| 40 | 41 | 42 | 43 |
	|____|____|____|    |
	|         |    |    |
ROW5	| 50      | 52 |    |
	|_________|____|____|

*/
