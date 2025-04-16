#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include "serial.h"
#include "timer.h"

#define INT_PARSING_FAIL -1

#define COMMAND_ARG_SPLIT ' '
#define LED_ARG_LENGTH 8
#define PARSE_LED_ARG_FAIL (1 << 8)

// Lengths include space after command name
#define LED_COMMAND_NAME_LENGTH     (3 + 1)
#define SERIAL_COMMAND_NAME_LENGTH  (6 + 1)
#define ONESHOT_COMMAND_NAME_LENGTH (7 + 1)
#define TIMER_COMMAND_NAME_LENGTH   (5 + 1)
#define LED_COMMAND_NAME     "led "
#define SERIAL_COMMAND_NAME  "serial "
#define ONESHOT_COMMAND_NAME "oneshot "
#define TIMER_COMMAND_NAME   "timer "

#define LED_FAIL_MESSAGE        "\nLED fail\n"
#define LED_FAIL_MESSAGE_LENGTH (sizeof(LED_FAIL_MESSAGE) - 1)
#define ONESHOT_ARG_FAIL_MESSAGE        "\nOneshot argument was not a number!\n"
#define ONESHOT_ARG_FAIL_MESSAGE_LENGTH (sizeof(ONESHOT_ARG_FAIL_MESSAGE) - 1)
#define INVALID_COMMAND_MESSAGE        "\nINVALID Command: "
#define INVALID_COMMAND_MESSAGE_LENGTH (sizeof(INVALID_COMMAND_MESSAGE) - 1)

// Enum for possible command types (otherwise INVALID)
enum command_type {
	SET_LED,
	SERIAL_ECHO,
	TRIGGER_ONESHOT,
	SET_TIMER,
	INVALID
};

// Finds a character in a string. Stops on any NULL '\0'.
// data:       The string
// length:     The maximum length to search
// terminator: The character to find
// Returns the index of the terminator or length if terminator was not found
uint32_t string_find(char* data, uint32_t length, char terminator);

// Checks if a string starts with a prefix
// data:          The string
// length:        The length of the string
// prefix:        The prefix to compare the string with
// prefix_length: The length of the prefix
// Returns true if the string starts with the prefix, otherwise false
bool starts_with(char* data,
				 uint32_t length,
				 char* prefix,
				 uint32_t prefix_length);

// Parses a string to determine the command type
// data: The string
// length: The length of the string
// Returns the command type or INVALID if none were parsed
enum command_type parse_command_type(char* data, uint32_t length);

// Parses the LED command arguments from a string to a binary number.
// args: The arguments
// length: The length of the arguments
// Returns the 8 bit number representing the parsed argument or a 9 bit number
// if parsing failed
int16_t parse_led_arg(char* args, uint32_t length);

// Parses the string as an unsigned int. Assumes base 10
// data:   The string
// length: The length of the string
// Returns the number as a decimal or INT_PARSING_FAIL if parsing failed
int32_t parse_unsigned_int(char* data, uint32_t length);

// The LED command.
// args:   The LED command arguments
// length: The length of the command arguments
void led_command(char* args, uint32_t length);

// The echo command.
// buf: The serial port buffer
void echo_command(SerialPortBuffer* buf);

// The oneshot command.
// args: The oneshot command arguments
// length: The length of the command arguments
void oneshot_command(char* args, uint32_t length);

// The timer command.
// args: The timer command arguments
// length: The length of the command arguments
void timer_command(char* args, uint32_t length);

// Parses a string and runs any associated commands. Intended to be used as
// the serial read completion function.
// buf: The serial port buffer
void parse_string(SerialPortBuffer* buf);

#endif // PARSER_H
