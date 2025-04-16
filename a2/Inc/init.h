#ifndef INIT_H
#define INIT_H
#include <stdint.h>

// enable the clock (Enable GPIOA, C and E)
void enable_clocks(void);

//initialise_board (init LED PE8-15 output)
void initialise_board(void);

// enable the interrupt (PA0 button press input)
void enable_interrupt(void);

//Setting the callback function when the button PA0 pressed
//callback is a function pointer of on_button_press
void setting_callback(void(*callback)(void));
#endif
