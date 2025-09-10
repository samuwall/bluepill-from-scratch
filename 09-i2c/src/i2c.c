/********************************************************************
 ** file         : i2c.c
 ** description  : 
 **
 **
 ********************************************************************/

#include <stdint.h>
#include "device.h"
#include "rcc.h"
#include "gpio.h"
#include "i2c7.h"
#include "SEGGER_RTT.h"

static void delay_ms(uint32_t ms) {
    uint32_t n = (ms * 72000) / 5;
    while (n--) {
        __asm__("nop");
    }
}

static void clock_setup(void) {

    set_sysclk_72mhz();
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN_;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN_;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN_;

}

static void gpio_setup(void) {

    /* PB6 -> AF open-drain 50MHz */
    GPIOB->CRL = (GPIOB->CRL & ~(GPIO_CRL_CNFMODE6_Msk)) | (GPIO_CNFMODE_OUTPUT_AF_OPENDRAIN_50MHZ << GPIO_CRL_CNFMODE6_Shft);

    /* PB7 -> AF open-drain 50MHz */
    GPIOB->CRL = (GPIOB->CRL & ~(GPIO_CRL_CNFMODE7_Msk)) | (GPIO_CNFMODE_OUTPUT_AF_OPENDRAIN_50MHZ << GPIO_CRL_CNFMODE7_Shft);

}

static void i2c_setup(void) {

    /* i2c peripheral must be disabled before configuring */
    I2C1->CR1 &= ~I2C_CR1_PE_;

    /* freq bits must be configured with APB clock frequency value, in this case APB1 is 36mhz */
    I2C1->CR2 = (I2C1->CR2 & ~(I2C_CR2_FREQ_Msk)) | (36 << I2C_CR2_FREQ_Shft);

    /* set fast mode (400khz) .. bc we can */
    I2C1->CCR |= I2C_CCR_FS_;

    /* for f_scl = 400khz and f_pclk1 = 36mhz and R_p = 4.7KOhm, the stm32f103x8 datasheet suggests a CCR of 0x1E */
    I2C1->CCR = (I2C1->CCR & ~(I2C_CCR_CCR_Msk)) | (0x1E << I2C_CCR_CCR_Shft);

    /* trise = "maximum duration of scl feedback loop in master mode" 
     * trise = (t_r(scl) / t_pclk1) + 1
     * according to datasheet, for an f_scl of 400khz, t_r(scl) = 300ns
     * t_pclk1 = periph clock period = 1 / 36mhz = 27.777ns = 28ns
     * trise = (300ns / 28ns) + 1 = 10.7 + 1 = 11.7 = (integer result) = 11
     */
    I2C1->TRISE = (I2C1->TRISE & ~(I2C_TRISE_TRISE_Msk)) | (11 << I2C_TRISE_TRISE_Shft);

    /* re-enable i2c peripheral */
    I2C1->CR1 |= I2C_CR1_PE_;

}

int main(void) {

    clock_setup();
    gpio_setup();
    i2c_setup();

    uint8_t read[16] = {0};
    uint8_t write[] = {0, 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', ' ', 'm', 'o', 'i'};
    uint8_t eeprom_slave_addr;
    uint8_t byte_address;

    eeprom_slave_addr = 0b1010000;  /* 24LC16: block 0 */
    byte_address = 0b00000000;      /* byte 0 */

    /* sequential read starting from block 0 byte 0 for `n = sizeof(read)` bytes */
    i2c7_write(I2C1, eeprom_slave_addr, &byte_address, sizeof(byte_address), I2C_NOSTOP);
    i2c7_read(I2C1, eeprom_slave_addr, read, sizeof(read));

    SEGGER_RTT_printf(0, "Initial EEPROM data: \n");
    for (size_t i = 0; i < sizeof(read); i++) {
        SEGGER_RTT_printf(0, "%3d ", read[i]);
    }
    SEGGER_RTT_printf(0, "\n\n");

    /* first byte sent is the byte/word address */
    write[0] = byte_address;

    /* page write the `write[]` array starting at block 0 byte 0 */
    i2c7_write(I2C1, eeprom_slave_addr, write, sizeof(write), I2C_STOP);

    /* wait for eeprom internal write cycle -- max 5ms */
    delay_ms(5);

    /* sequential read the same section of bytes */
    i2c7_write(I2C1, eeprom_slave_addr, &byte_address, sizeof(byte_address), I2C_NOSTOP);
    i2c7_read(I2C1, eeprom_slave_addr, read, sizeof(read));

    SEGGER_RTT_printf(0, "Modified EEPROM data: \n");
    for (size_t i = 0; i < sizeof(read); i++) {
        SEGGER_RTT_printf(0, "%3d ", read[i]);
    }
    SEGGER_RTT_printf(0, "\n\n");

    for (;;) {
        /* done */
    }

}
