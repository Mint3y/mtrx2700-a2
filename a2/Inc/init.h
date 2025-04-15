#ifndef INIT_H
#define INIT_H
#include <stdint.h>

void enable_clocks(void); // enable the clock

void initialise_board(void); //initialise_board (init the LED)

void enable_interrupt(void); // enable the interrupt

void setting_callback(void(*callback)(void)); //callback（on_button_press）

#endif
