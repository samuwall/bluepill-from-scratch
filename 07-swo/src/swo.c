/********************************************************************
 ** file         : swo.c
 ** description  : 
 **
 **
 ********************************************************************/

#include <stdint.h>
#include "device.h"
#include "rcc.h"
#include "gpio.h"

static void delay(uint32_t n) {
    while (n--) {
        __asm__("nop");
    }
}

static void clock_setup(void) {

    set_sysclk_72mhz();
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN_;

}

static void gpio_setup(void) {

    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNFMODE13_Msk)) |
                 (GPIO_CNFMODE_OUTPUT_GP_OPENDRAIN_2MHZ << GPIO_CRH_CNFMODE13_Shft);

    /* turn off LED */
    gpio_set(GPIOC, GPIO13);

}

static void blink() {
    gpio_toggle(GPIOC, GPIO13);
    delay(10000);
    gpio_toggle(GPIOC, GPIO13);
}

static void swo_send_char(char c) {
    
    while ((ITM->PORT[0].u8 & ITM_STIM_FIFONOTFULL_) == 0) ;
    ITM->PORT[0].u8 = c;

}

int main(void) {

    clock_setup();
    gpio_setup();

    const char *str = "hello world!";
    int i = 0;

    for (;;) {
        
        if (str[i] == '\0') {
            swo_send_char('\n');
            swo_send_char('\r');
            i = 0;
        }
        else {
            swo_send_char(str[i]);
            i++;
        }

        blink();
        delay(3e6);
    }

}

