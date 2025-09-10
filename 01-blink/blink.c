/***********************************************************************************
 ** file         : blink.c
 ** description  : blink PC13
 **
 **  goal                    register        address         bitfields
 **  -------------------------------------------------------------------
 **  enable GPIOC clock      RCC_APB2ENR     0x4002 1018     bit  4 -> 1 (IOPCEN -> enable)
 **  pc13->output mode       GPIOC_CRH       0x4001 1004     bit 21 -> 1 (MODE13 -> output 2MHz)
 **  pc13->float (LED OFF)   GPIOC_ODR       0x4001 100C     bit 13 -> 1 (ODR13  -> output 1)
 **  pc13->0v    (LED ON)    GPIOC_ODR       0x4001 100C     bit 13 -> 0 (ODR13  -> output 0)
 **
 **********************************************************************************/

#include <stdint.h>

#define RCC_APB2ENR *((volatile uint32_t *) 0x40021018)
#define GPIOC_CRH   *((volatile uint32_t *) 0x40011004)
#define GPIOC_ODR   *((volatile uint32_t *) 0x4001100C)

static void delay(uint32_t n) {
    while (n--) {
        __asm__("nop");
    }
}

void reset_handler(void) {

    RCC_APB2ENR = RCC_APB2ENR | (1 << 4);
    GPIOC_CRH = GPIOC_CRH | (1 << 20);

    for(;;) {

        GPIOC_ODR = GPIOC_ODR | (1 << 13);
        delay(500000);
        GPIOC_ODR = GPIOC_ODR & ~(1 << 13);
        delay(500000);

    }

}

void nmi_handler(void) {
    for(;;);
}

void hardfault_handler(void) {
    for(;;);
}

const uint32_t vector_table[]
__attribute__ ((section(".vectors"))) = {
    (uint32_t) 0x20005000,      /* top of stack = RAM origin + size (20KB) */
    (uint32_t) reset_handler,
    (uint32_t) nmi_handler,
    (uint32_t) hardfault_handler
};

