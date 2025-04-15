


#include "timb.h"
#include <stddef.h>
#include "stm32f303xc.h"


// set period at start, configure timer with period
// get_period and based on that period set a different period toggles between the two
// Different LED will be displayed for each period



static PatternForCallback pattern = 0;
static int period = 0;





void set_period(int delay_ms){
    period = delay_ms;
    TIM2->ARR = delay_ms;
    TIM2->CNT = 0;
}



int get_period(){
    return period;
}




// Initialise timer and trigger an interrupt for a call back function per delay period

void initiate_timer(int delay_ms, PatternForCallback CallBack){

	pattern = CallBack;

	period = delay_ms;



	TIM2->PSC = 7999;
	TIM2-> ARR = period;
	TIM2 -> CNT = 0;

    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->CR1 |= TIM_CR1_CEN;


}


// Interrupt service routine for call back function

void TIM2_IRQHandler(void){

	if(TIM2 -> SR & TIM_SR_UIF){

		// Clear Flag for next update event
	    TIM2->SR &= ~TIM_SR_UIF;


		if(pattern != 0){
		pattern(period);

		}
	}


}













