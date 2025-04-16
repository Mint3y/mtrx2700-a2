#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "stm32f303xc.h"


/*CALL BACK FUNCTIONS*/

// Function that traverses through a list of LED patterns upon each update event of the interrupt
typedef void (*PatternForCallback)();

// Function that changes LED pattern based on a different period
//typedef void (*PatternForNewPeriod)(int period);
typedef void (*PatternForNewPeriod)();
typedef void (*OneShotCallback)();


/*TIMER FUNCTIONS*/



// Function that configures timer and trigger interrupt for part a
void initialise_timer_a(int delay_ms, uint8_t * led_reg, PatternForCallback CallBack);


// Function that configures timer and trigger interrupt for part b
void initialise_timer_b(int delay_ms, PatternForNewPeriod CallBack);



// Configure timer for one shot event and trigger interrupt
void oneshot_timer_start(uint32_t delay_ms, OneShotCallback callback, uint8_t *led_reg);


void trigger_prescaler();


/*PATTERN FUNCTIONS*/


void change_pattern_a(void);
//void change_pattern_b(int period);
void change_pattern_b1(void);

void one_shot_pattern(void);

/*SETTER AND GETTER FUNCTIONS*/

void set_period(int delay);
int get_period();



/*TESTING FUNCTIONS*/

void testa(int period_value);
void testb(int period_value);
void testc(int oneshotdelay);

#endif


