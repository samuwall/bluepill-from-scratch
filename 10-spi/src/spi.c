/**********************************************************************************
 ** file         : spi.c
 ** description  : stm32f103 spi <--> ADXL345 accelerometer
 **
 **********************************************************************************/

#include <stdint.h>
#include "device.h"
#include "rcc.h"
#include "gpio.h"
#include "spim.h"
#include "SEGGER_RTT.h"

/* ADXL345 registers */
#define ADXL345_DEVID       0x00
#define ADXL345_POWER_CTL   0x2D
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0      0x32
#define ADXL345_DATAX1      0x33
#define ADXL345_DATAY0      0x34
#define ADXL345_DATAY1      0x35
#define ADXL345_DATAZ0      0x36
#define ADXL345_DATAZ1      0x37
#define ADXL345_FIFO_CTL    0x38

#define ADXL345_POWER_CTL_MEASURE_      (1 << 3)
#define ADXL345_DATA_FORMAT_LALIGN_     (1 << 2)
#define ADXL345_FIFO_CTL_MODE_STREAM    (1 << 7)

static void delay_ms(uint32_t ms) {
    uint32_t n = (ms * 72000) / 5;
    while (n--) {
        __asm__("nop");
    }
}

static void clock_setup(void) {
    set_sysclk_72mhz();
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN_;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN_;
}

static void gpio_setup(void) {

    /* SPI2:
     * pinout: datasheet pin definitions 
     * config: rm0008 p.167 table 24 */

    /* PB12 (CS) -> GP pupd 50MHz */
    GPIOB->CRH = (GPIOB->CRH & ~(GPIO_CRH_CNFMODE12_Msk)) | (GPIO_CNFMODE_OUTPUT_GP_PUSHPULL_50MHZ<< GPIO_CRH_CNFMODE12_Shft);

    /* PB13 (SPI2_SCK) -> AF pupd 50MHz */
    GPIOB->CRH = (GPIOB->CRH & ~(GPIO_CRH_CNFMODE13_Msk)) | (GPIO_CNFMODE_OUTPUT_AF_PUSHPULL_50MHZ << GPIO_CRH_CNFMODE13_Shft);

    /* PB14 (SPI2_MISO) -> input floating */
    GPIOB->CRH = (GPIOB->CRH & ~(GPIO_CRH_CNFMODE14_Msk)) | (GPIO_CNFMODE_INPUT_FLOAT << GPIO_CRH_CNFMODE14_Shft);

    /* PB15 (SPI2_MOSI) -> AF pupd 50MHz */
    GPIOB->CRH = (GPIOB->CRH & ~(GPIO_CRH_CNFMODE15_Msk)) | (GPIO_CNFMODE_OUTPUT_AF_PUSHPULL_50MHZ << GPIO_CRH_CNFMODE15_Shft);

    /* adxl datasheet: "if power is applied
     * to the ADXL345 before the clock polarity and phase of the host
     * processor are configured, the CS pin should be brought high before
     * changing the clock polarity and phase" */
    gpio_set(GPIOB, GPIO12);

}

static void spi_setup(void) {

    /* pulse reset signal for SPI2 peripheral 
     * TODO?: `rcc_pulse_rst(SPI2_RST)` */
    RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST_;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST_;

    /* at reset, configured as 2-line unidirectional full-duplex.
     * this is standard 4-wire SPI, so we'll keep it this way */

    /* otherwise, let's set our communication parameters */
    SPI2->CR1 = (SPI2->CR1 & ~SPI_CR1_BR_Msk) | (SPI_CR1_BR_FPCLK_DIV256);
    SPI2->CR1 |= SPI_CR1_CPOL_;
    SPI2->CR1 |= SPI_CR1_CPHA_;
    SPI2->CR1 &= ~SPI_CR1_DFF_;
    SPI2->CR1 &= ~SPI_CR1_LSBFIRST_;

    /* enable master mode */
    SPI2->CR1 |= SPI_CR1_MSTR_;

    /* manage CS manually in software */
    SPI2->CR1 |= SPI_CR1_SSM_;
    SPI2->CR2 &= ~SPI_CR2_SSOE_;

    /* for some reason, must set NSS high when manually controlling
     * CS via GPIO, or else MSTR and SPE will be cleared (rm0008 25.3.3).
     * the SSI bit is provided for this, it's meaningless unless SSM is set */
    SPI2->CR1 |= SPI_CR1_SSI_;

    /* enable peripheral */
    SPI2->CR1 |= SPI_CR1_SPE_;

}

static uint8_t adxl_read(uint8_t addr) {
    uint8_t data;

    /* pull CS low: select this slave */
    gpio_clear(GPIOB, GPIO12);

    /* send reg addr with r/w bit [7] set (read op) */
    spi_transfer(SPI2, addr | (1 << 7));

    /* receive reg data */
    data = spi_transfer(SPI2, 0);

    /* pull CS high: transaction complete */
    gpio_set(GPIOB, GPIO12);

    return data;
}

static void adxl_write(uint8_t addr, uint8_t data) {

    gpio_clear(GPIOB, GPIO12);

    /* send reg addr with r/w bit [7] cleared (write op) */
    spi_transfer(SPI2, addr);

    /* write reg data */
    spi_transfer(SPI2, data);

    gpio_set(GPIOB, GPIO12);

}

static void adxl_get_xyz(int16_t *x, int16_t *y, int16_t *z) {

    if (x) {
        *x = adxl_read(ADXL345_DATAX0) | (adxl_read(ADXL345_DATAX1) << 8);
    }
    if (y) {
        *y = adxl_read(ADXL345_DATAY0) | (adxl_read(ADXL345_DATAY1) << 8);
    }
    if (z) {
        *z = adxl_read(ADXL345_DATAZ0) | (adxl_read(ADXL345_DATAZ1) << 8);
    }
}

int main(void) {

    int16_t x = 0, y = 0, z = 0;
    uint8_t dev_id = 1;

    clock_setup();
    gpio_setup();
    spi_setup();

    /* adxl datasheet: 1.4ms typ turn-on time */
    delay_ms(2);

    /* if dev_id != 0xE5, the problem is most likely hardware-based.
     * try a diff breadboard, shorter wires, soldering, etc. */
    dev_id = adxl_read(ADXL345_DEVID);
    SEGGER_RTT_printf(0,"\n\ndevid: 0x%02X\n", dev_id);

    adxl_write(ADXL345_DATA_FORMAT, ADXL345_DATA_FORMAT_LALIGN_);
    adxl_write(ADXL345_FIFO_CTL, ADXL345_FIFO_CTL_MODE_STREAM);
    adxl_write(ADXL345_POWER_CTL, ADXL345_POWER_CTL_MEASURE_);

    SEGGER_RTT_printf(0, "x: %6d y: %6d z: %6d\n", x, y, z);

    for (;;) {
        delay_ms(10);
        adxl_get_xyz(&x, &y, &z);
        SEGGER_RTT_printf(0, "x: %6d y: %6d z: %6d\n", x, y, z);
    }

}
