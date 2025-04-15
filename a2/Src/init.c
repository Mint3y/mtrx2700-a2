#include "init.h"
#include "stm32f303xc.h"
#include <stdint.h>


//define the function pointer of the on_button_press & setting default value
//Prevent accidental triggering when no callback is assigned
static void (*on_button_press)(void) = 0x00;


//setting the callback function
//callback function will be called when the button PA0 pressed
void setting_callback(void(*callback)(void)){
	on_button_press = callback;
}


// enable the clocks for desired peripherals (GPIOA, C and E)
void enable_clocks(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOEEN;
}



// initialise the board(set LED PE8-15 OUTPUT)
//led_output_registers:setting all general purpose output mode of LED (MODER = 01)
void initialise_board(void) {
	// get a pointer to the second half word of the MODER register (for outputs pe8-15)
	uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1;
	*led_output_registers = 0x5555;
}
//Enable the external interrupt for PA0
//Trigger type: Rise-edge logic
void enable_interrupt(void) {
	// Disable the interrupts while messing around with the settings
	//  otherwise can lead to strange behaviour
	__disable_irq();

	// Enable the system configuration controller (SYSCFG in RCC)
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// External Interrupts details on large manual page 294)
	// PA0 is on interrupt EXTI0 large manual - page 250
	// EXTI0 in  SYSCFG_EXTICR1 needs to be 0x00 (SYSCFG_EXTICR1_EXTI0_PA)
	SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PA;

	//  Select EXTI0 interrupt on rising edge
	EXTI->RTSR |= EXTI_RTSR_TR0; // rising edge of EXTI line 0 (includes PA0)

	// set the interrupt from EXTI line 0 as 'not masked' - as in, enable it.
	EXTI->IMR |= EXTI_IMR_MR0;

	// Tell the NVIC module that EXTI0 interrupts should be handled
	NVIC_SetPriority(EXTI0_IRQn, 1);  // Set Priority
	NVIC_EnableIRQ(EXTI0_IRQn);

	// Re-enable all interrupts (now that we are finished)
	__enable_irq();
}


//This function will be called automatically when PA0 button pressed
void EXTI0_IRQHandler(void)
{
	// if the callback is Registration and call it
	if (on_button_press != 0x00) {
		on_button_press();
	}

	// reset the interrupt (so it doesn't keep firing until the next trigger)
	EXTI->PR |= EXTI_PR_PR0;
}
