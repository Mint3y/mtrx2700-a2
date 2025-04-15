#ifndef LED_H
#define LED_H
#include <stdint.h>

void chase_led(void); // chase_led

// Setting the led status
void set_led(uint8_t state);
// get the led status
void get_led(void);

#endif
