/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
// This is main file
#include "init.h"
#include "led.h"
#include <stdint.h>
#include "stm32f303xc.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

// store a pointer to the function that is called when a button is pressed
// set a default value of NULL so that it won't be called until the
// function pointer is defined


int main(void)
{
	//enable the clock
	enable_clocks();
	//initialise_board
	initialise_board();
	//call back setting
	setting_callback(&chase_led);
	// enable the interrupt for the button
	enable_interrupt();

    /* Loop forever */
	for(;;) {}
}
