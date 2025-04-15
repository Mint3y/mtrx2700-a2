

#include <stdint.h>
#include "stm32f303xc.h"
#include "tima.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif






// Enable clocks for peripherals and timers

void enable_clocks() {

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOEEN;



	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;


}


// Set mode for LED register

void initialise_board() {

	uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1;
	*led_output_registers = 0x5555;


}


void trigger_prescaler() {

	TIM2->ARR = 0x01;
	TIM2->CNT = 0x00;
	asm("NOP");
	asm("NOP");
	asm("NOP");
	TIM2->ARR = 0xffffffff;

}


// Call back function for changing LED patterns

void change_pattern(uint8_t * led_reg){


	static int count = 0;

	// LED pattern array
	uint8_t pattern_list[] = {0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00};

	*led_reg = pattern_list[count];

	// Count traverse through each index starting from 0 and loops back to start
	count = (count + 1 ) % 9 ;

}



// Change LED pattern every 2 seconds
int main(void)
{


	enable_clocks();
	initialise_board();


	uint8_t *led_output_register = ((uint8_t*)&(GPIOE->ODR)) + 1;



	initialise_timer(2000, led_output_register, change_pattern);
	for (;;) {}


}
