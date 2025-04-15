#ifndef INITA_H
#define INITA_H
#include <stdint.h>

void enable_clocks(void);

void initialise_board(void);

void enable_interrupt(void);

extern void (*on_button_press)(void);

#endif
