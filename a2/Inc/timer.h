#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef void(*Led)(uint8_t * Led_pattern);
typedef void(*OneShot)(uint8_t * Led_pattern);



typedef struct{

	uint32_t on_time;
	uint32_t off_time;

}PeriodValues;


void init_led_change(uint32_t on_time, uint32_t off_time, Led call_back);

void changing_pattern(volatile uint32_t *CNT, uint8_t * Led_output_register, uint8_t Led_pattern);

PeriodValues get_current_period(void);

void set_period(uint32_t on_time, uint32_t off_time);

void clock_tick_calculation(int ms_delay);

void one_shot_mode(int * activate_one_shot);

void one_shot(int delay, OneShot call_back, uint8_t * Led_output_register );

#endif
