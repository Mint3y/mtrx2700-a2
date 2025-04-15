#ifndef TIMB_H
#define TIMB_H

#include <stdint.h>

// Changes LED pattern based on current period
typedef void (*PatternForCallback)(int period);


typedef void (*Led)(uint8_t * Led_pattern);


// Configure timer and trigger interrupt
void initiate_timer(int delay_ms, PatternForCallback CallBack);


// Set and get current period
void set_period(int delay);
int get_period();



#endif
