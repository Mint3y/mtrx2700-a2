#include "timer.h"

static uint8_t *led_output_register = ((uint8_t*)&(GPIOE->ODR)) + 1;


static PatternForCallback pattern1 = 0;
static PatternForNewPeriod pattern2 = 0;

static OneShotCallback oneshot_callback = 0;

static uint8_t *Led_register = 0;
static int period = 0;
//static uint8_t *oneshot_led_reg = 0;

int mode = 0; 	// mode 1  = part a and mode 2 = part b



static struct {
	TIM_TypeDef* timer;
	void (*callback)();
} TIMER_STATE;

// Initialise the timer module.
void init_timer() {
	TIMER_STATE.timer = TIM2;
	TIMER_STATE.callback = 0x00;
}

// Enable interrupt requests for timer 2.
void enable_timer2_interrupts() {
	// Disable all interrupt requests while changing interrupt registers
	__disable_irq();

	// Set priority and enable interrupt requests for TIM2 in the NVIC
	NVIC_SetPriority(TIM2_IRQn, 5);
	NVIC_EnableIRQ(TIM2_IRQn);

	// Re-enable interrupt requests
	__enable_irq();

    // Enable capture/compare interrupt for TIM2
    TIM2->DIER |= TIM_DIER_CC1IE;
}

// Set the callback function that is run when the timer triggers.
// callback: The function to call when the timer interrupt triggers
void set_timer_callback(void (*callback)()) {
	TIMER_STATE.callback = callback;
}

// Allows the timer to count.
void resume_timer() {
	TIMER_STATE.timer->CR1 |= TIM_CR1_CEN;
}

// Stops the timer from counting.
void pause_timer() {
	TIMER_STATE.timer->CR1 &= ~TIM_CR1_CEN;
}

// Sets the timer counter back to 0.
void restart_timer() {
	TIMER_STATE.timer->CNT = 0;
}

// Sets the timer pre-scaler (number of clock ticks before timer counts).
// value: The value to set the pre-scaler to
void set_timer_prescaler(uint16_t value) {
	TIMER_STATE.timer->PSC = value;
}

// Enables the one-pulse timer option
void enable_timer_onepulse() {
	TIMER_STATE.timer->CR1 |= TIM_CR1_OPM;
}

// Disables the one-pulse timer option
void disable_timer_onepulse() {
	TIMER_STATE.timer->CR1 &= ~TIM_CR1_OPM;
}

// Sets the timer auto reload (value to count up to/down from). Period will be
// divided by the pre-scaler value.
// value: The value to set the auto reload register to
void set_timer_base_period(uint32_t value) {
	TIMER_STATE.timer->ARR = value;
}

void set_timer_oneshot_delay(uint32_t value) {
	set_timer_base_period(value);
	TIMER_STATE.timer->CCR1 = value;
}



void trigger_prescaler() {

	TIM2->ARR = 0x01;
	TIM2->CNT = 0x00;
	asm("NOP");
	asm("NOP");
	asm("NOP");
	TIM2->ARR = 0xffffffff;

}





// Initialise timer and trigger an interrupt for a call back function per delay period

void initialise_timer_a(int delay_ms, uint8_t *led_reg, PatternForCallback CallBack){

	mode = 1;

//	pattern1 = CallBack;
	set_timer_callback(CallBack);

	Led_register = led_reg;

	// Configure each timer cycle to 1ms
	TIM2->PSC = 7999;
	TIM2-> ARR = delay_ms;
	TIM2 -> CNT = 0;

//	//Enable Interrupt
//    TIM2->DIER |= TIM_DIER_UIE;
//    NVIC_EnableIRQ(TIM2_IRQn);

    //Enable Count
    TIM2->CR1 |= TIM_CR1_CEN;


}


void initialise_timer_b(int delay_ms, PatternForNewPeriod CallBack){

	mode = 2;

//	pattern2 = CallBack;
	set_timer_callback(CallBack);

	period = delay_ms;



	TIM2->PSC = 7999;
	TIM2-> ARR = period;
	TIM2 -> CNT = 0;

    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->CR1 |= TIM_CR1_CEN;


}



void oneshot_timer_start(uint32_t delay_ms, OneShotCallback callback, uint8_t *led_reg)
{
//    oneshot_callback = callback;
	set_timer_callback(callback);
    Led_register = led_reg;


    uint32_t ticks = delay_ms;


    TIM2->PSC = 7999;

    // Sets value for capture/compare event
    TIM2->CCR1 = ticks;

    // Set period of a single pulse
    TIM2->ARR = ticks;

    // Enable one-pulse mode
    TIM2->CR1 |= TIM_CR1_OPM;

    TIM2->DIER |= TIM_DIER_CC1IE;

    TIM2->CNT = 0;

    // Enable timer
    TIM2->CR1 |= TIM_CR1_CEN;

	NVIC_EnableIRQ(TIM2_IRQn);
}



// Interrupt service routine for call back function


void TIM2_IRQHandler(void) {
	// Update current state for interrupt
	if(TIM2 -> SR & TIM_SR_UIF){

		// Clear Flag for next update event
	    TIM2->SR &= ~TIM_SR_UIF;

	    TIMER_STATE.callback();
	    return;


//	    if(mode == 1){
//			if(pattern1 != 0){
//			pattern1(Led_register);
//
//			}
//
//	    }
//
//	    else if (mode == 2){
//
//	    	if(pattern2 != 0){
//				pattern2(period);
//TIM_SR_CC1IF
//			}
//
//	    }
	}

	// oneshot first pulse
//	 if (TIM2->SR & TIM_SR_CC1IF)
//	    {
//	        // Disable capture/compare flag
//	        TIM2->SR &= ~TIM_SR_CC1IF;
//
//		    TIMER_STATE.callback();
//		    return;
//
//	        if (oneshot_callback != 0)
//	        {
//	            oneshot_callback(Led_register);
//
//	        }
//
//	        // Disable the Capture/Compare 1 interrupt
//	        TIM2->DIER &= ~TIM_DIER_CC1IE;
//
//
//	 }
}





// Functions for part b
void set_period(int delay_ms){
    period = delay_ms;
    TIM2->ARR = delay_ms;
    TIM2->CNT = 0;
}



int get_period(){
    return period;
}





// Call back function for changing LED patterns

void change_pattern_a(){


	static int count = 0;

	// LED pattern array
	uint8_t pattern_list[] = {0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00};

	*led_output_register = pattern_list[count];

	// Count traverse through each index starting from 0 and loops back to start
	count = (count + 1 ) % 9 ;

}




void change_pattern_b(void){


	*led_output_register ^= 0xFF;

}


void one_shot_pattern(void)
{
    *led_output_register = 0xFF;
}








/*Testing Functions */

// Change LED pattern every 2 seconds
void testa(int period_value)
{

//	uint8_t *led_output_register = ((uint8_t*)&(GPIOE->ODR)) + 1;


	initialise_timer_a(period_value, led_output_register, change_pattern_a);
	for (;;) {}



}


void testb(int period_value){

	set_period(period_value);

	int period = get_period();

	initialise_timer_b(period, change_pattern_b);

	for(;;) {

		}
}


void testc(int oneshotdelay){

	uint32_t on_time = 2000;
	uint32_t off_time = 4000;

	uint8_t led_mask_pattern = 0b01010101;
	uint8_t *led_output_register = ((uint8_t *)&(GPIOE->ODR)) + 1;

//	// Flash a bit
//	*led_output_register = led_mask_pattern;
//	while (TIM2->CNT < on_time) { }
//	*led_output_register = 0x00;
//	while (TIM2->CNT < off_time) { }


//	oneshot_timer_start(oneshotdelay, one_shot_pattern, led_output_register);

//	while (1) {}
//

	for (;;)
	{

		TIM2->CNT = 0;
		TIM2->CR1 |= TIM_CR1_CEN;
		TIM2->PSC = 7999;
		trigger_prescaler();


		*led_output_register = led_mask_pattern;
		while (TIM2->CNT < on_time) { }

		*led_output_register = 0x00;
		while (TIM2->CNT < off_time) { }


		oneshot_timer_start(oneshotdelay, one_shot_pattern, led_output_register);

		// Delay to observe one shot led
		for (volatile uint32_t delay = 0; delay < 800000; delay++) { }

		for(;;) {}
	}

}




