


#include <stdint.h>
#include "stm32f303xc.h"
#include "timb.h"

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



// Alternate LED pattern based on period value

void change_pattern(int period){

	uint8_t *led_output_register = ((uint8_t*)&(GPIOE->ODR)) + 1;

	// LED on for 2 seconds
	if (period == 1000){
		*led_output_register = 0xFF;
		set_period(2000);
	}

	// LED off for 1 second
	else{
		*led_output_register = 0x00;
		set_period(1000);
	}

}


//

int main()
{
	enable_clocks();
	initialise_board();

	set_period(1000);

	int period = get_period();

	initiate_timer(period, change_pattern);

	for(;;) {

	    }

}

