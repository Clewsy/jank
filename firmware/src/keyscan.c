// The keyscan.h and keyscan.c files are intended to manage the hardware level.  I.e. correlate GPIO status to desired key-presses.
// Converting the desired key-presses into HID reports is handled by the Keyboard.c and Keyboard.h files.

#include "keyscan.h"

// Initialise the gpio for scanning rows and columns.
void keyscan_init(void)
{
	// Set rows as outputs and initialise all as high (disabled).
	ROWS_DDR |= ((1 << ROW0) | (1 << ROW1) | (1 << ROW2) | (1 << ROW3) | (1 << ROW4) | (1 << ROW5));
	ROWS_PORT |= ((1 << ROW0) | (1 << ROW1) | (1 << ROW2) | (1 << ROW3) | (1 << ROW4) | (1 << ROW5));

	// Set columns as inputs and enable pull-ups.
	COLS_DDR &= ~((1 << COL0) | (1 << COL1) | (1 << COL2) | (1 << COL3));
	COLS_PORT |= ((1 << COL0) | (1 << COL1) | (1 << COL2) | (1 << COL3));
}

// Parse the detected key and update the appropriate part of the report struct.
void handle_key(char key, keyscan_report_t *keyscan_report)
{
	// Media key scan values start at 0xF0, after the last keyboard modifier key scan.
	if(key > HID_KEYBOARD_SC_RIGHT_GUI)
	{
		// Convert the media key to a value from 0 to 10.
		key -= HID_MEDIACONTROLLER_SC_PLAY;

		// Shift a bit to the corresponding bit within the media_keys integer.
		keyscan_report->media_keys |= (1 << key);
	}

	// Modifier keys scan values start at 0xE0, after the last keyboard modifier key scan.
	else if(key > HID_KEYBOARD_SC_APPLICATION)
	{
		// Convert the media key to a value from 0 to 7.
		key -= HID_KEYBOARD_SC_LEFT_CONTROL;

		// Shift a bit to the corresponding bit within the modifier integer.
		keyscan_report->modifier |= (1 << key);
	}

	// Regular keys scan values range from 0x00 to 0x65.
	else  if(key > HID_KEYBOARD_SC_RESERVED)
	{
		// Skip array elements that already have a keyscan value written.
		uint8_t i = 0;
		while((keyscan_report->keys[i]) && (i < MAX_KEYS)) i++;

		// Only register the key if the maximum number of simultaneous keys is not reached.
		if(i < MAX_KEYS) keyscan_report->keys[i] = key;
	}
}

// Returns the address of a keyscan report which contains the key/modifier key-presses to be sent to the host.
void create_keyscan_report(keyscan_report_t *keyscan_report)
{
	// Start with a blank keyscan report.
	memset(keyscan_report, 0, sizeof(keyscan_report_t));

	// Loop through for each row.
	for(uint8_t r = 0; r < sizeof(key_row_array); r++)
	{
		// Set low current row (enable check).
		ROWS_PORT &= ~(1 << key_row_array[r]);

		// Wait until row is set low before continuing, otherwise column checks can be missed.
		while(!(~ROWS_PINS & (1 << key_row_array[r]))) {}

		// Loop through for each column in the current row.
		for(uint8_t c = 0; c < sizeof(key_col_array); c++)
		{
			// If the button in the current row and column is pressed, handle it.
			if(~COLS_PINS & (1 << key_col_array[c]))
			{
				// Determine desired keypresses.
				handle_key(pgm_read_byte(&KEYMAP[r][c]), keyscan_report);
			}
		}

		// Set high current row (disable check).
		ROWS_PORT |= (1 << key_row_array[r]);
	}
}

// Returns the address of a macro, i.e. first character in a string to be "typed".
// Note: only the first detected macro will be registered.  I.e. simultaneous macro key-presses is not possible.
const macro_t *scan_macro_keys(void)
{
	// Loop through for each row.
	for(uint8_t r = 0; r < sizeof(macro_row_array); r++)
	{
		// Set low current row (enable check).
		ROWS_PORT &= ~(1 << macro_row_array[r]);

		// Wait until row is set low before continuing, otherwise column checks can be missed.
		while(!(~ROWS_PINS & (1 << macro_row_array[r]))) {}

		// Loop through for each column in the current row.
		for(uint8_t c = 0; c < sizeof(macro_col_array); c++)
		{
			// If the button in the current row and column is pressed, handle it.
			if(~COLS_PINS & (1 << macro_col_array[c]))
			{
				// Returns the address of the desired macro array.
				return(&MACROMAP[r][c][0]);
			}
		}

		// Set high current row (disable check).
		ROWS_PORT |= (1 << macro_row_array[r]);
	}

	// If no macro key pressed, return 0.
	return(0);
}

// Converts a character to a keyboard scancode.
uint8_t char_to_code(char key)
{
	switch (key)
	{
		case 'A' ... 'Z' :	return ((uint8_t)key - 61);
		case 'a' ... 'z' :	return ((uint8_t)key - 93);
		case '1' ... '9' :	return ((uint8_t)key - 19);
		case '0' :		return (HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS);
		case '!' :		return (HID_KEYBOARD_SC_1_AND_EXCLAMATION);
		case '@' :		return (HID_KEYBOARD_SC_2_AND_AT);
		case '#' :		return (HID_KEYBOARD_SC_3_AND_HASHMARK);
		case '$' :		return (HID_KEYBOARD_SC_4_AND_DOLLAR);
		case '%' :		return (HID_KEYBOARD_SC_5_AND_PERCENTAGE);
		case '^' :		return (HID_KEYBOARD_SC_6_AND_CARET);
		case '&' :		return (HID_KEYBOARD_SC_7_AND_AMPERSAND);
		case '*' :		return (HID_KEYBOARD_SC_8_AND_ASTERISK);
		case '(' :		return (HID_KEYBOARD_SC_9_AND_OPENING_PARENTHESIS);
		case ')' :		return (HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS);
		case '\n':		return (HID_KEYBOARD_SC_ENTER);
		case '\e':		return (HID_KEYBOARD_SC_ESCAPE);
		case '\b':		return (HID_KEYBOARD_SC_BACKSPACE);
		case '\t':		return (HID_KEYBOARD_SC_TAB);
		case ' ' :		return (HID_KEYBOARD_SC_SPACE);
		case '-' :
		case '_' :		return (HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE);
		case '=' :
		case '+' :		return (HID_KEYBOARD_SC_EQUAL_AND_PLUS);
		case '[' :
		case '{' :		return (HID_KEYBOARD_SC_OPENING_BRACKET_AND_OPENING_BRACE);
		case ']' :
		case '}' :		return (HID_KEYBOARD_SC_CLOSING_BRACKET_AND_CLOSING_BRACE);
		case '\\':
		case '|' :		return (HID_KEYBOARD_SC_BACKSLASH_AND_PIPE);
		case ';' :
		case ':' :		return (HID_KEYBOARD_SC_SEMICOLON_AND_COLON);
		case '\'':
		case '"' :		return (HID_KEYBOARD_SC_APOSTROPHE_AND_QUOTE);
		case '`' :
		case '~' :		return (HID_KEYBOARD_SC_GRAVE_ACCENT_AND_TILDE);
		case ',' :
		case '<' :		return (HID_KEYBOARD_SC_COMMA_AND_LESS_THAN_SIGN);
		case '.' :
		case '>' :		return (HID_KEYBOARD_SC_DOT_AND_GREATER_THAN_SIGN);
		case '/' :
		case '?' :		return (HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK);

		default :		return (0x00);	//If there is no character match "fail" by sending a "NO_KEY".
	}
}

// Determines if a shift modifier is required to "type" a character (e.g. for capital letters).
bool upper_case_check(char key)
{
	switch (key)
	{
		case '!' ... '&' :
		case '(' ... '+' :
		case ':' :
		case '<' :
		case '>' ... 'Z' :
		case '^' ... '_' :
		case '{' ... '~' :	return true;

		default :		return false;
	}
}
