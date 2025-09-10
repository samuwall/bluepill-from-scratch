/**********************************************************************************
 ** file         : tim.c
 ** description  : `delay_ms()` function based on TIM peripheral
 **
 **
 **********************************************************************************/

#include <stdint.h>
#include "device.h"
#include "rcc.h"
#include "gpio.h"
#include "SEGGER_RTT.h"

static void clock_setup(void) {

    set_sysclk_48mhz();
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN_;

}

static void gpio_setup(void) {

    GPIOC->CRH = (GPIOC->CRH & ~GPIO_CRH_CNFMODE13_Msk) | 
        (GPIO_CNFMODE_OUTPUT_GP_PUSHPULL_2MHZ << GPIO_CRH_CNFMODE13_Shft);

}

static void tim_setup(void) {

    /* datasheet fig. 2: if APB1 prescaler != 1, TIM2_clk = APB1 * 2 = 24 * 2 = 48MHz
     * 48MHz / 48 = 1MHz = 1 million counts a sec = 1uS per count
     * PSC = div - 1 = 47 
     */
    TIM2->PSC = 47;

    /* generate update event:
     * the PSC is only applied upon an update event, which is only triggered by 
     * a counter over/underflow, which we might (will) never have. so, we force
     * an update event through software by setting this bit.
     */
    TIM2->EGR |= TIM_EGR_UG_;

    TIM2->CNT = 0;

    /* enable TIM2 counter */
    TIM2->CR1 |= TIM_CR1_CEN_;
}

static void delay_ms(uint32_t ms) {

    while (ms--) {
        TIM2->CNT = 0;
        /* loop for 1000 counts => 1ms */
        while (TIM2->CNT < 1000);
    }

}

int main(void) {

    clock_setup();
    gpio_setup();
    tim_setup();

    for (;;) {

        delay_ms(1000);
        gpio_toggle(GPIOC, GPIO13);
        
    }
}
