#include <stdint.h>
#include "inita.h"
#include "stm32f303xc.h"
#include "leda.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

// store a pointer to the function that is called when a button is pressed
// set a default value of NULL so that it won't be called until the
// function pointer is defined


int main(void)
{
	enable_clocks();
	initialise_board();
	on_button_press = &chase_led;
	// enable the interrupt for the button
	enable_interrupt();

    /* Loop forever */
	for(;;) {}
}
