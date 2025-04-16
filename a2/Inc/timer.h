#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>


/*CALL BACK FUNCTIONS*/

// Function that traverses through a list of LED patterns upon each update event of the interrupt
typedef void (*PatternForCallback)(uint8_t * );

// Function that changes LED pattern based on a different period
typedef void (*PatternForNewPeriod)(int period);

typedef void (*OneShotCallback)(uint8_t *);


/*TIMER FUNCTIONS*/



// Function that configures timer and trigger interrupt for part a
void initialise_timer_a(int delay_ms, uint8_t * led_reg, PatternForCallback CallBack);


// Function that configures timer and trigger interrupt for part b
void initialise_timer_b(int delay_ms, PatternForNewPeriod CallBack);



// Configure timer for one shot event and trigger interrupt
void oneshot_timer_start(uint32_t delay_ms, OneShotCallback callback, uint8_t *led_reg);


void trigger_prescaler();


/*PATTERN FUNCTIONS*/


void change_pattern_a(uint8_t * led_reg);
void change_pattern_b(int period);




/*SETTER AND GETTER FUNCTIONS*/

void set_period(int delay);
int get_period();



/*TESTING FUNCTIONS*/

void testa(void);
void testb(void);
void testc(int OneShotDelay);

#endif


