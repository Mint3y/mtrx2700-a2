
#include "led.h"
#include "stm32f303xc.h"
#include <stdint.h>


//pointer to the upper 8-bit of GPIOE->ODR (PE8-15 LED)
//each bit control one led
static uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;


// the function is shift the LED pattern
void chase_led(void){

	// left shift the led by one bit
	*led_register <<= 1;
	//if the result touch 0, reset to the 0x01 (PE08 ON)
	if (*led_register == 0) {
		*led_register = 1;
	}
	// Update the led status to GPIOE
	*((uint8_t*)&(GPIOE->ODR) + 1) = *led_register;
}


// Setting the led status (PE8–PE15) with the 8-bit value
//status 8-bit value: each bits repersents of one LED; ON=1, OFF=0
void set_led(uint8_t state){
	*led_register =state;   //Update the led internal state
	*(((uint8_t*)&(GPIOE->ODR)) + 1)= state; // send the led bitmasks to GPIOE
}


// get the led status
//return: the 8-bit led status value
 void get_led(void){
	 return *led_register; // return the current state of the led
 }
