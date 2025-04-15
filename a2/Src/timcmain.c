
#include "timc.h"
#include "stm32f303xc.h"



// Enable clocks for peripherals and timers

void enable_clocks(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOEEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}


// Set mode for LED register

void initialise_board(void)
{
    uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1;
    *led_output_registers = 0x5555;
}


void trigger_prescaler(void)
{
    TIM2->ARR = 0x01;
    TIM2->CNT = 0x00;
    asm("NOP");
    asm("NOP");
    asm("NOP");
    TIM2->ARR = 0xffffffff;
}



void one_shot_pattern(uint8_t *led_reg)
{
    *led_reg = 0xFF;
}

int main(void)
{

	uint32_t on_time = 2000;
		uint32_t off_time = 4000;

		enable_clocks();
		initialise_board();


		uint8_t led_mask_pattern = 0b01010101;
		uint8_t *led_output_register = ((uint8_t *)&(GPIOE->ODR)) + 1;



	// Call one shot after every on/off cycle
    for (;;)
    {


    	TIM2->CNT = 0;

    	// Configure timer for normal toggling

    	TIM2->CR1 |= TIM_CR1_CEN;
		TIM2->PSC = 7999;
		trigger_prescaler();

        *led_output_register = led_mask_pattern;

        while (TIM2->CNT < on_time) { }

        *led_output_register = 0x00;
        while (TIM2->CNT < off_time) { }


        oneshot_timer_start(1000, one_shot_pattern, led_output_register);

        // Delay to observe one shot led
        for (volatile uint32_t delay = 0; delay < 800000; delay++) { }
    }

    return 0;
}
