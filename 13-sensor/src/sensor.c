/**********************************************************************************
 ** file         : sensor.c
 ** description  : paw3395 test program
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
#include "spim.h"
#include "paw3395.h"

#include "SEGGER_RTT.h"

#define MAX_BURST_SIZE  12
#define BURST_SIZE      12

static void delay_ms(uint32_t ms) {
    while (ms--) {
        TIM2->CNT = 0;
        while (TIM2->CNT < 1000);
    }
}

void print_binary(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        uint8_t bit = (byte >> i) & 1;
        SEGGER_RTT_printf(0, "%d", bit);
    }
    SEGGER_RTT_WriteString(0, "\n");
}

static void clock_setup(void) {

    set_sysclk_72mhz();
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN_;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN_;

}

static void gpio_setup(void) {

    /* rm0008 9.1.11 table 25
     * SPI GPIO configurations */
    
    /* PA4 (SPI1_CS) -> GP pupd 50MHz */
    GPIOA->CRL = (GPIOA->CRL & ~GPIO_CRL_CNFMODE4_Msk) | (GPIO_CNFMODE_OUTPUT_GP_PUSHPULL_50MHZ << GPIO_CRL_CNFMODE4_Shft);
    gpio_set(GPIOA, GPIO4);

    /* PA5 (SPI1_SCK) -> AF pupd 50MHz */
    GPIOA->CRL = (GPIOA->CRL & ~GPIO_CRL_CNFMODE5_Msk) | (GPIO_CNFMODE_OUTPUT_AF_PUSHPULL_50MHZ << GPIO_CRL_CNFMODE5_Shft);

    /* PA6 (SPI1_MISO) -> input floating */
    GPIOA->CRL = (GPIOA->CRL & ~GPIO_CRL_CNFMODE6_Msk) | (GPIO_CNFMODE_INPUT_FLOAT << GPIO_CRL_CNFMODE6_Shft);

    /* PA7 (SPI1_MOSI) -> AF pupd 50MHz  */
    GPIOA->CRL = (GPIOA->CRL & ~GPIO_CRL_CNFMODE7_Msk) | (GPIO_CNFMODE_OUTPUT_AF_PUSHPULL_50MHZ << GPIO_CRL_CNFMODE7_Shft);

}

static void tim_setup(void) {

    /* 72MHz / 72 = 1MHz */
    TIM2->PSC = 71;

    /* generate update event: apply PSC */
    TIM2->EGR |= TIM_EGR_UG_;

    /* enable TIM2 counter */
    TIM2->CR1 |= TIM_CR1_CEN_;

}

static void spi_setup(void) {

    /* at reset, configured as 2-line unidirectional full-duplex.
     * this is standard 4-wire SPI, so we'll keep it this way */

    /* otherwise, let's set our communication parameters */
    SPI1->CR1 = (SPI1->CR1 & ~SPI_CR1_BR_Msk) | (SPI_CR1_BR_FPCLK_DIV8);
    SPI1->CR1 |= SPI_CR1_CPOL_;
    SPI1->CR1 |= SPI_CR1_CPHA_;
    SPI1->CR1 &= ~SPI_CR1_DFF_;
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST_;

    /* enable master mode */
    SPI1->CR1 |= SPI_CR1_MSTR_;

    /* manage CS manually in software */
    SPI1->CR1 |= SPI_CR1_SSM_;
    SPI1->CR2 &= ~SPI_CR2_SSOE_;

    /* for some reason, must set NSS high when manually controlling
     * CS via GPIO, or else MSTR and SPE will be cleared (rm0008 25.3.3).
     * the SSI bit is provided for this, it's meaningless unless SSM is set */
    SPI1->CR1 |= SPI_CR1_SSI_;

    /* enable peripheral */
    SPI1->CR1 |= SPI_CR1_SPE_;

}

static uint8_t paw_read(uint8_t addr) {

    uint8_t data;

    /* pull CS low: select this slave */
    gpio_clear(GPIOA, GPIO4);

    /* send addr we wish to read with MSB cleared (read op) */
    spi_transfer(SPI1, addr);

    /* receive reg data */
    data = spi_transfer(SPI1, 0);

    /* pull CS high: transaction complete */
    gpio_set(GPIOA, GPIO4);

    return data;
}

static void paw_write(uint8_t addr, uint8_t data) {

    /* pull CS low: select this slave */
    gpio_clear(GPIOA, GPIO4);

    /* send addr we wish to read with MSB set (write op) */
    spi_transfer(SPI1, addr | (1 << 7));

    /* write reg data */
    spi_transfer(SPI1, data);

    /* pull CS high: transaction complete */
    gpio_set(GPIOA, GPIO4);

}

static void paw_modify(uint8_t addr, uint8_t clearmask, uint8_t setmask) {
    uint8_t reg = paw_read(addr);
    reg = (reg & ~clearmask) | (setmask);
    paw_write(addr, reg);
}

static void paw_motion_burst(uint8_t *byte, uint8_t len) {

    /* receive up to 12 bytes */
    uint8_t burst_len = (len > MAX_BURST_SIZE) ? MAX_BURST_SIZE : len;

    /* CS low */
    gpio_clear(GPIOA, GPIO4);

    /* start by sending motion_burst addr */
    spi_transfer(SPI1, PAW3395_MOTION_BURST);

    /* receive burst data */
    for (uint8_t i = 0; i < burst_len; i++) {
        byte[i] = spi_transfer(SPI1, 0);
    }

    /* CS high */
    gpio_set(GPIOA, GPIO4);

}

int main(void) {

    uint8_t p_id = 1;
    uint8_t burst_data[BURST_SIZE] = {0};

    clock_setup();
    gpio_setup();
    tim_setup();
    spi_setup();

    /* give paw3395 plenty of time to initialize */
    delay_ms(100);

    /* should be 0x51 */
    p_id = paw_read(PAW3395_PRODUCT_ID);
    SEGGER_RTT_printf(0, "\n\nPRODUCT_ID: 0x%02X\n", p_id);

    /* disable rest mode */
    paw_modify(PAW3395_PERFORMANCE, 0, PAW3395_PERFORMANCE_AWAKE_);

    for (;;) {

        delay_ms(100);
        paw_motion_burst(burst_data, sizeof(burst_data));

        SEGGER_RTT_WriteString(0, "\n\n");
        for (uint8_t i = 0; i < BURST_SIZE; i++) {
            SEGGER_RTT_printf(0, "byte[%02d]: ", i);
            print_binary(burst_data[i]);
            SEGGER_RTT_WriteString(0, "\n");
        }
        
    }

}
