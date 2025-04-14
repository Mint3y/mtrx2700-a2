#include "parser.h"

int32_t string_find(char* data, char terminator) {
	// Iterate until terminator or end of string character
	uint32_t i = 0;
	while (*data != terminator && *data != '\0') {
		++i;
		++data;
	}

	// Terminator was not found, reached end of string
	if (*data == '\0' && terminator != '\0') {
		return REACHED_END_OF_STRING;
	}

	return i;
}

bool starts_with(char* data, uint32_t length, char* prefix) {
	// Prefix is longer than string
	uint32_t prefix_length = string_find(prefix, '\0');
	if (prefix_length > length) {
		return false;
	}

	// End index is the shorter of the two lengths
	uint32_t end = length > prefix_length ? prefix_length : length;
	for (int i = 0; i < end; ++i) {
		if (data[i] != prefix[i]) {
			return false;
		}
	}

	// No mismatches found
	return true;
}

enum command_type parse_command_type(char* data) {
	// Get the length of the first word in the string
	uint32_t first_word_length = string_find(data, COMMAND_ARG_SPLIT);
	if (first_word_length == REACHED_END_OF_STRING) {
		return INVALID;
	}

	// Determine the command type using prefixes
	if (starts_with(data, first_word_length, LED_COMMAND)) {
		return SET_LED;
	}
	else if (starts_with(data, first_word_length, SERIAL_COMMAND)) {
		return SERIAL_ECHO;
	}
	else if (starts_with(data, first_word_length, ONESHOT_COMMAND)) {
		return TRIGGER_ONESHOT;
	}
	else if (starts_with(data, first_word_length, ONESHOT_COMMAND)) {
		return SET_TIMER;
	}

	// No valid command found
	return INVALID;
}

int16_t parse_led_arg(char* data) {
	// Ensure argument is 8 characters ('bits') long
	if (string_find(data, '\0') != LED_ARG_LENGTH) {
		return PARSE_LED_ARG_FAIL; // Return 9 bits of data
	}

	// Convert the string of 1s and 0s to an integer
	int32_t i = 0;
	int16_t result = 0;
	while (i < LED_ARG_LENGTH) {
		// String is '1', set the i'th bit of the result
		if (data[i] == '1') {
			result |= 1 << (LED_ARG_LENGTH - 1 - i);
		}

		// String was not '1' or '0'
		else if (data[i] != '0') {
			return PARSE_LED_ARG_FAIL;
		}
	}

	return result;
}

void parse_string(char* data, uint32_t length) {
	// Validate command type
	enum command_type type = parse_command_type(data);
	if (type == INVALID) {
		return;
	}

	// Obtain start of command arguments
	char* command_args = data + string_find(data, COMMAND_ARG_SPLIT);

	// Execute the command based on type
	switch (type) {
	case SET_LED: {
		// Validate led argument
		int16_t led_arg = parse_led_arg(command_args);
		if (led_arg == PARSE_LED_ARG_FAIL) {
			// Display a debug message and set LEDs to 0x00
			// TODO
		}

		// Set LEDs
		// TODO

		break;
	}

	case SERIAL_ECHO:
		// Echo TODO
		break;

	case TRIGGER_ONESHOT:
		// Trigger TODO
		break;

	case SET_TIMER:
		// Set timer TODO
		break;
	}
}
