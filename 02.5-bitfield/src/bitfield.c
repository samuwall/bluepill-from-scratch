/********************************************************************
 ** file         : bitfield.c
 ** description  : blink an LED using struct bitfields
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

    RCC->APB2ENR.bit.IOPCEN = 1;

    GPIOC->CRH.bit.CNF_MODE13 = GPIO_CNFMODE_OUTPUT_GP_OPENDRAIN_2MHZ;

    for(;;) {

        GPIOC->ODR.bit.ODR13 = 1;
        delay(500000);
        GPIOC->ODR.bit.ODR13 = 0;
        delay(500000);

    }

}

