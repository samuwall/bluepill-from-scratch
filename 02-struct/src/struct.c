/********************************************************************
 ** file         : struct.c
 ** description  : blink an LED using structs and macros
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

int main(void) {

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN_;

    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNFMODE13_Msk)) |
                 (GPIO_CNFMODE_OUTPUT_GP_OPENDRAIN_2MHZ << GPIO_CRH_CNFMODE13_Shft);

    for(;;) {

        GPIOC->ODR |= GPIO13;
        delay(500000);
        GPIOC->ODR &= ~GPIO13;
        delay(500000);

    }

}

