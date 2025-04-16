#ifndef LED_H
#define LED_H
#include <stdint.h>

// chase_led
//clock-cycle to shift LED by 1 step when the button pressed
void chase_led(void);


// Setting the led status (PE8–PE15) with the 8-bit value
//status 8-bit value: each bits repersents of one LED; ON=1, OFF=0
void set_led(uint8_t state);


// get the led status
//return: the 8-bit led status value
uint8_t get_led(void);

#endif
