


#include "tima.h"
#include <stddef.h>
#include "stm32f303xc.h"



static PatternForCallback pattern = 0;
static uint8_t *Led_register = 0;


// Initialise timer and trigger an interrupt for a call back function per delay period

void initialise_timer(int delay_ms, uint8_t *led_reg, PatternForCallback CallBack){

	pattern = CallBack;

	Led_register = led_reg;

	// Configure each timer cycle to 1ms
	TIM2->PSC = 7999;
	TIM2-> ARR = delay_ms;
	TIM2 -> CNT = 0;

	//Enable Interrupt
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    //Enable Count
    TIM2->CR1 |= TIM_CR1_CEN;


}


// Interrupt service routine for call back function

void TIM2_IRQHandler(void){

	if(TIM2 -> SR & TIM_SR_UIF){

		// Clear Flag for next update event
	    TIM2->SR &= ~TIM_SR_UIF;


		if(pattern != 0){
		pattern(Led_register);

		}
	}


}




