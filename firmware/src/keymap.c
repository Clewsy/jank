#include "keymap.h"

// Define the physical row and column pins on the microcontroller to be scanned for key presses.
const uint8_t key_row_array[]	= {ROW1, ROW2, ROW3, ROW4, ROW5};
const uint8_t key_col_array[]	= {COL0, COL1, COL2, COL3};
const uint8_t macro_row_array[]	= {ROW0};
const uint8_t macro_col_array[]	= {COL0, COL1, COL2, COL3};

// Number pad key definitions.
// KEY_XY where X:Row Number, Y:Column Number.
#define KEY_1_0 HID_KEYBOARD_SC_NUM_LOCK
#define KEY_1_1 HID_KEYBOARD_SC_KEYPAD_SLASH
#define KEY_1_2 HID_KEYBOARD_SC_KEYPAD_ASTERISK
#define KEY_1_3 HID_KEYBOARD_SC_KEYPAD_MINUS
#define KEY_2_0 HID_KEYBOARD_SC_KEYPAD_7_AND_HOME
#define KEY_2_1 HID_KEYBOARD_SC_KEYPAD_8_AND_UP_ARROW
#define KEY_2_2 HID_KEYBOARD_SC_KEYPAD_9_AND_PAGE_UP
#define KEY_2_3 HID_KEYBOARD_SC_KEYPAD_PLUS
#define KEY_3_0 HID_KEYBOARD_SC_KEYPAD_4_AND_LEFT_ARROW
#define KEY_3_1 HID_KEYBOARD_SC_KEYPAD_5
#define KEY_3_2 HID_KEYBOARD_SC_KEYPAD_6_AND_RIGHT_ARROW
#define KEY_3_3 0x00	// No key here.
#define KEY_4_0 HID_KEYBOARD_SC_KEYPAD_1_AND_END
#define KEY_4_1 HID_KEYBOARD_SC_KEYPAD_2_AND_DOWN_ARROW
#define KEY_4_2 HID_KEYBOARD_SC_KEYPAD_3_AND_PAGE_DOWN
#define KEY_4_3 HID_KEYBOARD_SC_KEYPAD_ENTER
#define KEY_5_0 HID_KEYBOARD_SC_KEYPAD_0_AND_INSERT
#define KEY_5_1 0x00	// No key here.
#define KEY_5_2 HID_KEYBOARD_SC_KEYPAD_DOT_AND_DELETE
#define KEY_5_3 0x00	// No key here.

// The key map array - for regular key strokes including media control keys.
const char KEYMAP[NUM_KEY_ROWS][NUM_KEY_COLS] PROGMEM = {
	//Col 0  Col 1   Col 2   Col 3
	{KEY_1_0, KEY_1_1, KEY_1_2, KEY_1_3}, // Row 1
	{KEY_2_0, KEY_2_1, KEY_2_2, KEY_2_3}, // Row 2
	{KEY_3_0, KEY_3_1, KEY_3_2, KEY_3_3}, // Row 3
	{KEY_4_0, KEY_4_1, KEY_4_2, KEY_4_3}, // Row 4
	{KEY_5_0, KEY_5_1, KEY_5_2, KEY_5_3}  // Row 5
};

// The macro map array - for key strokes that are mapped as macros.
const macro_t MACROMAP[NUM_MACRO_ROWS][NUM_MACRO_COLS][MAX_MACRO_ACTIONS] PROGMEM = 
{
	{ //Row0
		{ //Col0
			{M_KEYS, {HID_KEYBOARD_SC_F12}},	// This macro is just the same as hitting the F12 key.
		},
		{ //Col1
			{M_KEYS, {HID_KEYBOARD_SC_LEFT_GUI}},	// This macro will go to the specified url in a new firefox tab.
			{M_WAIT, {3}},
			{M_STRING, "firefox" },
			{M_KEYS, {HID_KEYBOARD_SC_ENTER}},
			{M_WAIT, {1}},
			{M_KEYS, {HID_KEYBOARD_SC_LEFT_CONTROL, HID_KEYBOARD_SC_T}},
			{M_STRING, "https://clews.pro/projects/jank.php" },
			{M_KEYS, {HID_KEYBOARD_SC_ENTER}},
		},
		{ //Col2
			{M_STRING, "      _\n"},	// This macro will enter a series of strings to create some ascii art.
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
			{M_STRING, "Bender is Great!" },	// This macro will type a string of characters then hit enter.
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
