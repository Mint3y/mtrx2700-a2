#include "parser.h"
#include "timer.h"

// Finds a character in a string. Stops at given length.
// data:       The string
// length:     The maximum length to search
// terminator: The character to find
// Returns the index of the terminator or length if terminator was not found
uint32_t string_find(char* data, uint32_t length, char terminator) {
	// Iterate string
	for (int i = 0; i < length; ++i) {
		// Terminator was found
		if (data[i] == terminator) {
			return i;
		}
	}

	return length;
}

// Checks if a string starts with a prefix
// data:          The string
// length:        The length of the string
// prefix:        The prefix to compare the string with
// prefix_length: The length of the prefix
// Returns true if the string starts with the prefix, otherwise false
bool starts_with(char* data,
				 uint32_t length,
				 char* prefix,
				 uint32_t prefix_length) {
	// Prefix is longer than string
	if (prefix_length > length) {
		return false;
	}

	// See if prefix matches start of string
	for (int i = 0; i < prefix_length; ++i) {
		if (data[i] != prefix[i]) {
			return false;
		}
	}

	// No mismatches found
	return true;
}

// Parses a string to determine the command type
// data: The string
// length: The length of the string
// Returns the command type or INVALID if none were parsed
enum command_type parse_command_type(char* data, uint32_t length) {
	// Get the length of the first word in the string (including space)
	uint32_t first_word_length = string_find(data, length, COMMAND_ARG_SPLIT) + 1;

	// Determine the command type using prefixes
	if (starts_with(data,
					first_word_length,
					LED_COMMAND_NAME,
					LED_COMMAND_NAME_LENGTH)) {
		return SET_LED;
	}
	else if (starts_with(data,
						 first_word_length,
						 SERIAL_COMMAND_NAME,
						 SERIAL_COMMAND_NAME_LENGTH)) {
		return SERIAL_ECHO;
	}
	else if (starts_with(data,
						 first_word_length,
						 ONESHOT_COMMAND_NAME,
						 ONESHOT_COMMAND_NAME_LENGTH)) {
		return TRIGGER_ONESHOT;
	}
	else if (starts_with(data,
						 first_word_length,
						 TIMER_COMMAND_NAME,
						 TIMER_COMMAND_NAME_LENGTH)) {
		return SET_TIMER;
	}

	// No valid command found
	return INVALID;
}

// Parses the LED command arguments from a string to a binary number.
// args: The arguments
// length: The length of the arguments
// Returns the 8 bit number representing the parsed argument or a 9 bit number
// if parsing failed
int16_t parse_led_arg(char* args, uint32_t length) {
	// Ensure argument is exactly 8 characters ('bits') long
	if (length != LED_ARG_LENGTH) {
		return PARSE_LED_ARG_FAIL; // Return 9 bits of data
	}

	// Convert the string of 1s and 0s to an integer
	int32_t i = 0;
	int16_t result = 0;
	while (i < LED_ARG_LENGTH) {
		// String is '1', set the i'th bit of the result
		if (args[i] == '1') {
			result |= 1 << (LED_ARG_LENGTH - 1 - i);
		}

		// String was not '1' or '0'
		else if (args[i] != '0') {
			return PARSE_LED_ARG_FAIL;
		}
	}

	return result;
}

// Parses the string as an unsigned int. Assumes base 10
// data:   The string
// length: The length of the string
// Returns the number as a decimal or INT_PARSING_FAIL if parsing failed
int32_t parse_unsigned_int(char* data, uint32_t length) {
	uint32_t base = 10;
	uint32_t digit = 1;
	int32_t result = 0;

	// Iterate string backwards
	for (int i = length - 1; i >= 0; --i) {
		// String is not numeric
		if (data[i] < '0' || data[i] > '9') {
			return INT_PARSING_FAIL;
		}

		// Sum digits, take into account digit multiplier
		result += digit * (data[i] - '0');
		digit *= base;
	}

	return result;
}

// The LED command.
// args: The LED command arguments
// length: The length of the command arguments
void led_command(char* args, uint32_t length) {
	// Validate led argument
	int16_t led_arg = parse_led_arg(args, length);
	if (led_arg == PARSE_LED_ARG_FAIL) {
		// Display a debug message and set LEDs to 0x00 (maybe we dont do this)
		// alternative debug: have all leds flash 3 times before reverting to their previous state
		// TODO
	}

	// Set LEDs
	// TODO
}

// The echo command.
// buf: The serial port buffer
void echo_command(SerialPortBuffer* buf) {
	// Get an open transmission buffer, fail if none are available
	SerialPortBuffer* transmit_buf = get_open_transmit_buffer();
	if (transmit_buf == 0x00) {
		return;
	}

	// Link the receive buffer to the transmit buffer
	transmit_buf->buffer_ref = buf;

	// Set start and end indices of transmit
	transmit_buf->index = SERIAL_COMMAND_NAME_LENGTH;
	transmit_buf->length = buf->length;

	begin_transmit_ready();
}

// The oneshot command.
// args: The oneshot command arguments
// length: The length of the command arguments
void oneshot_command(char* args, uint32_t length) {
	// Oneshot TODO

	int32_t delay = parse_unsigned_int(args, length);  // e.g 1000

	if(delay == INT_PARSING_FAIL){

		return;
	}



}

// The timer command.
// args: The timer command arguments
// length: The length of the command arguments
void timer_command(char* args, uint32_t length) {
	// Set timer TODO


	// pls use parse_unsigned_int(char* data)



}

// Parses a string and runs any associated commands. Intended to be used as
// the serial read completion function.
// buf: The serial port buffer
void parse_string(SerialPortBuffer* buf) {
	// Read command type
	enum command_type type = parse_command_type(buf->buffer, buf->length);

	// Execute the command based on type
	switch (type) {
	case SET_LED:
		led_command(buf->buffer + LED_COMMAND_NAME_LENGTH,
					buf->length - LED_COMMAND_NAME_LENGTH);
		buf->ready = true;
		break;

	case SERIAL_ECHO:
		echo_command(buf);
		break;

	case TRIGGER_ONESHOT:
		oneshot_command(buf->buffer + ONESHOT_COMMAND_NAME_LENGTH,
					    buf->length - ONESHOT_COMMAND_NAME_LENGTH);
		buf->ready = true;
		break;

	case SET_TIMER:
		timer_command(buf->buffer + TIMER_COMMAND_NAME_LENGTH,
					  buf->length - TIMER_COMMAND_NAME_LENGTH);
		buf->ready = true;
		break;

	default:
		buf->ready = true;
		break;
	}
}
