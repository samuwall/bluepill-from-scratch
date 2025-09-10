/**********************************************************************************
 ** file         : spdt.c
 ** description  : SPDT 2-pin 0-latency software debounce
 **
 ** board        : f103 + paw3395 eval board
 **
 **
 **********************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "device.h"
#include "rcc.h"
#include "gpio.h"
#include "SEGGER_RTT.h"

/* gets modified by ISRs, so we declare it as volatile */
volatile uint8_t l_click = 0;
volatile uint8_t r_click = 0;

static void clock_setup(void) {

    set_sysclk_72mhz();
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN_;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN_;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN_;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN_; /* for configuring EXTI line mapping */

}

static void gpio_setup(void) {

    GPIOC->CRH = (GPIOC->CRH & ~GPIO_CRH_CNFMODE13_Msk) | (GPIO_CNFMODE_OUTPUT_GP_PUSHPULL_50MHZ << GPIO_CRH_CNFMODE13_Shft);
    gpio_set(GPIOC, GPIO13);

    /* PA10 (L_NC): input with pull-up */
    GPIOA->CRH = (GPIOA->CRH & ~GPIO_CRH_CNFMODE10_Msk) | (GPIO_CNFMODE_INPUT_PUPD << GPIO_CRH_CNFMODE10_Shft);
    GPIOA->ODR |= GPIO10;

    /* PA9 (L_NO): input with pull-up */
    GPIOA->CRH = (GPIOA->CRH & ~GPIO_CRH_CNFMODE9_Msk)  | (GPIO_CNFMODE_INPUT_PUPD << GPIO_CRH_CNFMODE9_Shft);
    GPIOA->ODR |= GPIO9;

    /* PB12 (R_NC): input with pull-up */
    GPIOB->CRH = (GPIOB->CRH & ~GPIO_CRH_CNFMODE12_Msk) | (GPIO_CNFMODE_INPUT_PUPD << GPIO_CRH_CNFMODE12_Shft);
    GPIOB->ODR |= GPIO12;

    /* PA8 (R_NO): input with pull-up */
    GPIOA->CRH = (GPIOA->CRH & ~GPIO_CRH_CNFMODE8_Msk)  | (GPIO_CNFMODE_INPUT_PUPD << GPIO_CRH_CNFMODE8_Shft);
    GPIOA->ODR |= GPIO8;

}

static void exti_setup(void) {
    
    /* map EXTI lines 8-10 to PA8-10 */
    AFIO->EXTICR[2] = (AFIO->EXTICR[2] & ~(AFIO_EXTICR3_EXTI8_Msk
                                          | AFIO_EXTICR3_EXTI9_Msk
                                          | AFIO_EXTICR3_EXTI10_Msk))
                                        | (AFIO_EXTICR3_EXTI8_PA8 
                                          | AFIO_EXTICR3_EXTI9_PA9
                                          | AFIO_EXTICR3_EXTI10_PA10);

    /* map EXTI line 12 to PB12 */
    AFIO->EXTICR[3] = (AFIO->EXTICR[3] & ~AFIO_EXTICR4_EXTI12_Msk) | (AFIO_EXTICR4_EXTI12_PB12);

    /* unmask line 9 (L_NO) for now, keep line 10 masked (L_NC) */
    EXTI->IMR |=  EXTI9;
    EXTI->IMR &= ~EXTI10;

    /* unmask line 8 (R_NO) for now, keep line 12 masked (R_NC) */
    EXTI->IMR |=  EXTI8;
    EXTI->IMR &= ~EXTI12;

    /* enable falling-edge triggers */
    EXTI->FTSR |= EXTI8;
    EXTI->FTSR |= EXTI9;
    EXTI->FTSR |= EXTI10;
    EXTI->FTSR |= EXTI12;

    /* clear any potential spurious pending bits */
    EXTI->PR = EXTI8 | EXTI9 | EXTI10 | EXTI12;

    /* enable interrupts at NVIC level */
    NVIC->ISER[NVIC_EXTI9_5_IRQ / 32] = (1 << (NVIC_EXTI9_5_IRQ % 32));
    NVIC->ISER[NVIC_EXTI15_10_IRQ / 32] = (1 << (NVIC_EXTI15_10_IRQ % 32));

}


int main(void) {

    SEGGER_RTT_WriteString(0, RTT_CTRL_CLEAR);

    clock_setup();
    gpio_setup();
    exti_setup();

    for (;;) {

    }
    
}

void exti9_5_isr() {

    if (EXTI->PR & EXTI9) {
        
        /* disable PA9 (L_NO), enable PA10 (L_NC) */
        EXTI->IMR &= ~EXTI9;
        EXTI->IMR |=  EXTI10;

        /* clear pending bits */
        EXTI->PR = EXTI9 | EXTI10;

        l_click = 1;

        SEGGER_RTT_WriteString(0, "l_click: \x1B[1;31m1\x1B[0m\n");
        gpio_clear(GPIOC, GPIO13);

    }

    if (EXTI->PR & EXTI8) {

        /* disable PA8 (R_NO), enable PB12 (R_NC) */
        EXTI->IMR &= ~EXTI8;
        EXTI->IMR |=  EXTI12;

        /* clear pending bits */
        EXTI->PR = EXTI8 | EXTI12;

        r_click = 1;

        SEGGER_RTT_WriteString(0, "r_click: \x1B[4;42m1\x1B[0m\n");
        gpio_clear(GPIOC, GPIO13);

    }

}

void exti15_10_isr() {

    if (EXTI->PR & EXTI10) {
        
        /* disable PA10 (L_NC), enable PA9 (L_NO) */
        EXTI->IMR &= ~EXTI10;
        EXTI->IMR |=  EXTI9;

        /* clear pending bits */
        EXTI->PR = EXTI9 | EXTI10;

        l_click = 0;

        SEGGER_RTT_WriteString(0, "l_click: 0\n");
        gpio_set(GPIOC, GPIO13);

    }

    if (EXTI->PR & EXTI12) {

        /* disable PB12 (R_NC), enable PA8 (R_NO) */
        EXTI->IMR &= ~EXTI12;
        EXTI->IMR |=  EXTI8;

        /* clear pending bits */
        EXTI->PR = EXTI8 | EXTI12;

        r_click = 0;

        SEGGER_RTT_WriteString(0, "r_click: 0\n");
        gpio_set(GPIOC, GPIO13);

    }

}
