/**********************************************************************************
 ** file         : gpio.h
 ** description  : 
 **
 **********************************************************************************/

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "device.h"

void gpio_set(GPIO_T *gpioport, uint16_t gpios);
void gpio_clear(GPIO_T *gpioport, uint16_t gpios);
void gpio_toggle(GPIO_T *gpioport, uint16_t gpios);

#endif