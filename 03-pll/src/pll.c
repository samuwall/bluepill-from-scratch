/********************************************************************
 ** file         : pll.c
 ** description  : set sysclk to 72MHz using HSE+PLL, blink LED
 **
 **
 ********************************************************************/

#include <stdint.h>
#include "device.h"

static void delay(uint32_t n) {
    while (n--) {
        __asm__("nop");
    }
}

static void set_sysclk_72mhz(void) {

    /* enable HSE (8 MHz), loop until ready */
    RCC->CR |= RCC_CR_HSEON_;
    while (!(RCC->CR & RCC_CR_HSERDY_));

    /* configure prescalers and PLL */
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_HPRE_Msk     |
                               RCC_CFGR_PPRE1_Msk    |
                               RCC_CFGR_PPRE2_Msk    |
                               RCC_CFGR_ADCPRE_Msk   |
                               RCC_CFGR_USBPRE_Msk   |
                               RCC_CFGR_PLLSRC_Msk   |
                               RCC_CFGR_PLLXTPRE_Msk |
                               RCC_CFGR_PLLMUL_Msk))
                               
                             | RCC_CFGR_HPRE_NODIV        /* AHB prescaler:  /1     */
                             | RCC_CFGR_PPRE1_DIV2        /* APB1 prescaler: /2     */
                             | RCC_CFGR_PPRE2_NODIV       /* APB2 prescaler: /1     */
                             | RCC_CFGR_ADCPRE_DIV6       /* ADC prescaler:  /6     */
                             | RCC_CFGR_USBPRE_PLLDIV15   /* USB prescaler:  /1.5   */
                             | RCC_CFGR_PLLSRC_HSE        /* PLLSRC   -> HSE (8MHz) */
                             | RCC_CFGR_PLLXTPRE_HSE      /* PLLXTPRE -> HSE no div */
                             | RCC_CFGR_PLLMUL_9;         /* PLLMUL   -> 9x         */

    /* set flash latency to 2 wait states, as per RM0008 p58 for 72MHz SYSCLK */
    FLASH_ACR = (FLASH_ACR & ~(FLASH_ACR_LATENCY_Msk)) | (FLASH_ACR_LATENCY_2WAIT);

    /* enable PLL, loop until ready */
    RCC->CR |= RCC_CR_PLLON_;
    while (!(RCC->CR & RCC_CR_PLLRDY_));

    /* set SYSCLK source as PLL */
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_SW_Msk)) | (RCC_CFGR_SW_PLLCLK);

    /* loop until SYSCLK source is PLL at hw level */
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLLCLK);

}

static void clock_setup(void) {

    set_sysclk_72mhz();
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN_;

}

static void gpio_setup(void) {

    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNFMODE13_Msk)) |
                 (GPIO_CNFMODE_OUTPUT_GP_OPENDRAIN_2MHZ << GPIO_CRH_CNFMODE13_Shft);

}

int main(void) {

    clock_setup();
    gpio_setup();
    
    for(;;) {

        GPIOC->ODR |= GPIO13;
        delay(500000);
        GPIOC->ODR &= ~GPIO13;
        delay(500000);

    }

}

