#ifndef TIMC_H
#define TIMC_H

#include <stdint.h>


// Turn all LEDs on
typedef void (*OneShotCallback)(uint8_t *);


// Configure timer for one shot event and trigger interrupt
void oneshot_timer_start(uint32_t delay_ms, OneShotCallback callback, uint8_t *led_reg);

#endif



