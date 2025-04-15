#include "timc.h"
#include "stm32f303xc.h"


static OneShotCallback oneshot_callback = 0;
static uint8_t *oneshot_led_reg = 0;

void oneshot_timer_start(uint32_t delay_ms, OneShotCallback callback, uint8_t *led_reg)
{
    oneshot_callback = callback;
    oneshot_led_reg = led_reg;


    uint32_t ticks = delay_ms;


    TIM2->PSC = 7999;

    // Sets value for capture/compare event
    TIM2->CCR1 = ticks;

    // Set period of a single pulse
    TIM2->ARR = ticks;

    // Enable one-pulse mode
    TIM2->CR1 |= TIM_CR1_OPM;

    // Enable capture/compare interrupt
    TIM2->DIER |= TIM_DIER_CC1IE;

    TIM2->CNT = 0;

    // Enable timer
    TIM2->CR1 |= TIM_CR1_CEN;

    // Enable interrupt

    NVIC_EnableIRQ(TIM2_IRQn);

}

// Interrupt Service Routine

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_CC1IF)
    {
        // Disable capture/compare flag
        TIM2->SR &= ~TIM_SR_CC1IF;


        if (oneshot_callback != 0)
        {
            oneshot_callback(oneshot_led_reg);

        }

        // Disable the Capture/Compare 1 interrupt
        TIM2->DIER &= ~TIM_DIER_CC1IE;


    }
}
