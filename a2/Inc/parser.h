#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdbool.h>

#define REACHED_END_OF_STRING -1

#define COMMAND_ARG_SPLIT " "
#define LED_ARG_LENGTH 8
#define PARSE_LED_ARG_FAIL 0b100000000
#define LED_COMMAND "led"
#define SERIAL_COMMAND "serial"
#define ONESHOT_COMMAND "oneshot"
#define TIMER_COMMAND "timer"

enum command_type {
	SET_LED,
	SERIAL_ECHO,
	TRIGGER_ONESHOT,
	SET_TIMER,
	INVALID
};

uint32_t string_find(char* data, char terminator);

bool starts_with(char* data, uint32_t length, char* prefix);

enum command_type examine_string(char* data, uint32_t length);

int16_t parse_led_arg(char* data);

void parse_string(char* data, uint32_t length);

#endif // PARSER_H
