#include "keyscan.h"

// Initialise the gpio for scanning rows and columns.
void keyscan_init(void)
{
	// Set keys as inputs and enable pull-ups.
	KEYS_DDR &= ~((1 << KEY_1) | (1 << KEY_2) | (1 << KEY_3) | (1 << KEY_4));
	KEYS_PORT |= ((1 << KEY_1) | (1 << KEY_2) | (1 << KEY_3) | (1 << KEY_4));
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

void create_keyscan_report(keyscan_report_t *keyscan_report)
{
	// Start with a blank keyscan report.
	memset(keyscan_report, 0, sizeof(keyscan_report_t));

	// Loop through for each key.
	for(uint8_t k = 0; k < sizeof(KEY_PIN_ARRAY); k++)
	{
		// If the current key k is pressed.
		if(~KEYS_PINS & (1 << KEY_PIN_ARRAY[k]))
		{
			handle_key(pgm_read_byte(&KEY_MAP[k]), keyscan_report);
		}
	}
}

// Returns the address of a macro, i.e. first character in a string to be "typed".
// Note: only the first detected macro will be registered.  I.e. simultaneous macro key-presses is not possible.
const char *scan_macro_keys(void)
{

	static const char NO_MACRO[] PROGMEM = "";	// Needed when no macro key is pressed.

	// Loop through for each macro key.
	for(uint8_t m = 0; m < sizeof(MACRO_PIN_ARRAY); m++)
	{
		// If the macro key m is pressed, return the address of the macro string mapped in "keymap.c".
		if(~KEYS_PINS & (1 << MACRO_PIN_ARRAY[m])) return(&MACRO_MAP[m][0]);
	}
	return(&NO_MACRO[0]);	// Return the address of a blank array when no macro key is pressed.
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

// Determines if a shift modifier is required to reproduce a character.
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
