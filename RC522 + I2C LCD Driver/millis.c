#include "stm32f4xx.h"
#include "millis.h"

static volatile uint64_t ms = 0; //compteur global

void millis_init(void)
{
    // SysTick -> interruption toutes les 1 ms 
    SysTick->LOAD = (SystemCoreClock / 1000) - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

uint64_t millis(void)
{
    uint64_t current;
    __disable_irq();
    current = ms;
    __enable_irq();
    return current;
}

void SysTick_Handler(void)
{
    ms++;
}
