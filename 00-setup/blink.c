/***********************************************************************************
 ** file         : blink.c
 ** description  : blink PC13
 **
 **
 **********************************************************************************/

#include <stdint.h>

#define RCC_APB2ENR *((volatile uint32_t *) 0x40021018)
#define GPIOC_CRH   *((volatile uint32_t *) 0x40011004)
#define GPIOC_ODR   *((volatile uint32_t *) 0x4001100C)

static void delay(volatile uint32_t n) {
    while (n--);
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

/* vector table
 * only 4 required entries, per https://developer.arm.com/documentation/ddi0337/e/Exceptions/Resets/Vector-Table-and-Reset
 */
const uint32_t vector_table[]
__attribute__ ((section(".vectors"))) = {
    (uint32_t) 0x20005000,      /* top of stack = RAM origin + size (20KB) */
    (uint32_t) reset_handler,
    (uint32_t) nmi_handler,
    (uint32_t) hardfault_handler
};

