/**********************************************************************************
 ** file         : gpio.c
 ** description  : gpio operations
 **
 **********************************************************************************/

#include <stdint.h>
#include "device.h"
#include "gpio.h"

void gpio_set(GPIO_T* gpioport, uint16_t gpios) {
    gpioport->BSRR = gpios;
}

void gpio_clear(GPIO_T* gpioport, uint16_t gpios) {
    gpioport->BSRR = (gpios << 16);
}

void gpio_toggle(GPIO_T* gpioport, uint16_t gpios) {
    uint32_t odr = gpioport->ODR;
    gpioport->BSRR = ((odr & gpios) << 16) | (~odr & gpios);
}

