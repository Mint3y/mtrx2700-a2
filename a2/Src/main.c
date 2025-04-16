/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f303xc.h"

#include "init.h"
#include "serial.h"
#include "led.h"

#include "parser.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int io_test(void)
{
	//enable the clock
	enable_clocks();

	//initialise_board
	initialise_board();

	// Set the callback function to be called when the button is press (PA0)
	// When button is pressed, chase_led() will be invoked
	setting_callback(&chase_led);

	// enable the interrupt for the button
	enable_interrupt();

    /* Loop forever */
	for(;;) {}
}

void full_integration() {
	// Serial module initialisation
	init_serial();
	init_usart();
	init_serial_port_16bit(BAUD_9600,
	                       get_usart1_port(),
						   parse_string);
	enable_usart1_interrupts();
	enable_usart1_receive_interrupt();

	while (1) {}
}

int main(void) {
//    test_serial();
//	test_serial_interrupt();
	full_integration();

    /* Loop forever */
	for(;;);
}
