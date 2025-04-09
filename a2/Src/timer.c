


#include "timer.h"
#include <stddef.h>
#include "stm32f303xc.h"



// Create a timer function that takes in the delay period and a callback function
// What callback function do
//on (1sec) off (1sec) instead of off state, callback will turn on the first bit only



typedef void(*Led)(uint8_t * Led_pattern);
typedef void(*OneShot)(uint8_t * Led_pattern);

static uint32_t on_duration = 0;
static uint32_t off_duration = 0;
static Led timer_callback = NULL;

static int shot_mode = 1;


//Initializing variables
void init_led_change(uint32_t on_time, uint32_t off_time, Led call_back){

	on_duration = on_time;
	off_duration = off_time;
	timer_callback = call_back;
}


void set_period(uint32_t on_time, uint32_t off_time){ 				  // set function
	on_duration = on_time;
	off_duration = off_time;
}


void changing_pattern(volatile uint32_t *CNT, uint8_t * Led_output_register, uint8_t Led_pattern){



	// turn on the LEDs
	*Led_output_register = Led_pattern;

	while (*CNT < on_duration) {} ; 								// LED on for 800 000 ticks

	timer_callback(Led_output_register);							// Turn off all LEDS

	while (*CNT < off_duration) {};									// Leave LED off for 800 000 ticks

}


PeriodValues get_current_period(void){								// get function

	PeriodValues duration;
	duration.on_time = on_duration;
	duration.off_time = off_duration;

	return duration;

}



void one_shot_mode(int * activate_one_shot){

	if(activate_one_shot != NULL){
		*activate_one_shot = shot_mode;
	}
}




void clock_tick_calculation(int ms_delay){

	on_duration = (ms_delay * 800000) / 1000;

}



void one_shot(int delay, OneShot call_back, uint8_t * Led_output_register ){

	clock_tick_calculation(delay);
	TIM2->CNT = 0;
	while (TIM2->CNT < on_duration) {} ;

	call_back(Led_output_register);
	TIM2->CNT = 0;
	off_duration = 0xC3500;
	while (TIM2->CNT < off_duration) {}
	shot_mode = 0;
}








