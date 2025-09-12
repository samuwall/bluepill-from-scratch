/**********************************************************************************
 ** file         : delay.c
 ** description  : 
 **
 **
 **********************************************************************************/

#include <stdint.h>
#include "device.h"
#include "delay.h"

/* assumes free-running 1MHz tim_clk 
 */
void delay_ms(uint32_t ms) {
    while (ms--) {
        TIM2->CNT = 0;
        while (TIM2->CNT < 1000);
    }
}

/* assumes free-running 1MHz tim_clk 
 */
void delay_us(uint16_t us) {
    TIM2->CNT = 0;
    while (TIM2->CNT < us);
}
