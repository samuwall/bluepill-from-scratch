/********************************************************************
 ** file         : uart.c
 ** description  : demonstrate USART peripheral
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

static void usart_setup(void) {

    USART2->BRR = 36e6 / 115200;    /* set baud rate 115200: USARTDIV = (p_clk / baud) */
    USART2->CR1 = (USART2->CR1 & ~(USART_CR1_M_Msk)) | (USART_CR1_M_8BITS << USART_CR1_M_Shft);                         /* 8 data bits */
    USART2->CR2 = (USART2->CR2 & ~(USART_CR2_STOP_Msk)) | (USART_CR2_STOP_1BIT << USART_CR2_STOP_Shft);                 /* 1 stop bit */
    USART2->CR1 = (USART2->CR1 & ~(USART_CR1_MODE_Msk)) | (USART_CR1_MODE_TX << USART_CR1_MODE_Shft);                   /* TX mode */
    USART2->CR1 = (USART2->CR1 & ~(USART_CR1_PARITY_Msk)) | (USART_CR1_PARITY_NONE << USART_CR1_PARITY_Shft);           /* parity: none*/
    USART2->CR3 = (USART2->CR3 & ~(USART_CR3_FLOWCNTRL_Msk)) | (USART_CR3_FLOWCNTRL_NONE << USART_CR3_FLOWCNTRL_Shft);  /* flowcontrol: none */

    /* enable usart */
    USART2->CR1 |= USART_CR1_UE_;

}

static void clock_setup(void) {

    set_sysclk_72mhz();
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN_;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN_;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN_;

}

static void gpio_setup(void) {

    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNFMODE13_Msk)) |
                 (GPIO_CNFMODE_OUTPUT_GP_OPENDRAIN_2MHZ << GPIO_CRH_CNFMODE13_Shft);

    GPIOA->CRL = (GPIOA->CRL & ~(GPIO_CRL_CNFMODE2_Msk)) |
                 (GPIO_CNFMODE_OUTPUT_AF_PUSHPULL_50MHZ << GPIO_CRL_CNFMODE2_Shft);

}

static void usart_send_blocking(uint32_t data) {

    /* wait until data has been transferred into shift register */
    while (!(USART2->SR & USART_SR_TXE_));

    /* send data */
    USART2->DR = data & USART_DR_DR_Msk;
}

static void blink(void) {

    GPIOC->ODR ^= GPIO13;
    delay(50000);
    GPIOC->ODR ^= GPIO13;

}

int main(void) {

    clock_setup();
    gpio_setup();
    usart_setup();

    const char *str = "hello world!";
    int i = 0;
    
    for (;;) {
        if (str[i] == '\0') {
            usart_send_blocking('\n');
            usart_send_blocking('\r');
            i = 0;
        }
        else {
            usart_send_blocking(str[i]);
            i++;
        }

        blink(); /* indicate data sent */
        delay(6e6);
    }

}

