#ifndef TIMA_H
#define TIMA_H

#include <stdint.h>


// Function that traverses through a list of LED patterns upon each update event of the interrupt
typedef void (*PatternForCallback)(uint8_t * );


// Function that configures timer and trigger interrupt
void initialise_timer(int delay_ms, uint8_t * led_reg, PatternForCallback CallBack);


#endif
