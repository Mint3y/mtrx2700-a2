
#include "led.h"
#include "stm32f303xc.h"
#include <stdint.h>

static uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;;
//led chase
void chase_led(void){


	*led_register <<= 1; // left shift the led by one bit
	//if the result touch 0, set to the 0x01
	if (*led_register == 0) {
		*led_register = 1;
	}
	*((uint8_t*)&(GPIOE->ODR) + 1) = *led_register;// Update the led status to GPIOE
}
// Setting the led pattern
void set_led(uint8_t state){
	*led_register =state;   //Update the led internal state
	*(((uint8_t*)&(GPIOE->ODR)) + 1)= state; // send the led bitmasks to GPIOE
}
// get led current status
 void get_led(void){
	 return *led_register; // return the current state of the led
 }
