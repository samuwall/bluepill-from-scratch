/********************************************************************
 ** file         : rtt.c
 ** description  : 
 **
 **
 ********************************************************************/

#include <stdint.h>
#include "device.h"
#include "rcc.h"
#include "gpio.h"
#include "SEGGER_RTT.h"

static void delay(uint32_t n) {
    while (n--) {
        __asm__("nop");
    }
}

static void clock_setup(void) {
    set_sysclk_72mhz();
}

int main(void) {

    clock_setup();

    for (;;) {
        
        SEGGER_RTT_printf(0, "hello world!\n");
        delay(3e6);

    }

}

