/**********************************************************************************
 ** file         : i2c7.c
 ** description  : generic i2c operations (stm32f103)
 **
 **
 **********************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "device.h"
#include "i2c7.h"

void i2c7_stop(I2C_T *I2Cx) {

    /* generate stop condition */
    I2Cx->CR1 |= I2C_CR1_STOP_;

    /* make sure stop is cleared by hardware b4 returning */
    while (I2Cx->CR1 & I2C_CR1_STOP_);

}

void i2c7_write(I2C_T *I2Cx, uint8_t slave_addr, const uint8_t *data, size_t n, int stop) {

    while (I2Cx->SR2 & I2C_SR2_BUSY_);

    I2Cx->CR1 |= I2C_CR1_START_;

    /* wait until SB (start bit) is set, signaling a start condition has been generated. */
    while (!(I2Cx->SR1 & I2C_SR1_SB_));

    /* send 7 bit slave address (ADDR_LSB = r/w) */
    I2Cx->DR = ((slave_addr << 1) | ADDR_LSB_WRITE);

    /* wait until end of addr transmission (slave sent ack) */
    while (!(I2Cx->SR1 & I2C_SR1_ADDR_));

    /* clear ADDR bit by reading SR2 */
    (void) I2Cx->SR2;

    for (size_t i = 0; i < n; i++) {
        I2Cx->DR = data[i];
        while (!(I2Cx->SR1 & I2C_SR1_BTF_));
    }

    if (stop) {
        i2c7_stop(I2Cx);
    }

}

void i2c7_read(I2C_T *I2Cx, uint8_t slave_addr, uint8_t *buf, size_t n) {

    /* generate start */
    I2Cx->CR1 |= I2C_CR1_START_;

    /* enable ACK */
    I2Cx->CR1 |= I2C_CR1_ACK_;

    /* wait until SB (start bit) is set, signaling a start condition has been generated. */
    while (!(I2Cx->SR1 & I2C_SR1_SB_));

    /* send 7 bit slave address (ADDR_LSB = r/w) */
    I2Cx->DR = ((slave_addr << 1) | ADDR_LSB_READ);

    /* wait until end of addr transmission (slave sent ack) */
    while (!(I2Cx->SR1 & I2C_SR1_ADDR_));

    /* clear ADDR bit by reading SR2 */
    (void) I2Cx->SR2;

    for (size_t i = 0; i < n; i++) {

        if (i == n - 1) {
            /* disable ACK */
            I2Cx->CR1 &= ~I2C_CR1_ACK_;
        }

        /* wait for DR not empty */
        while (!(I2Cx->SR1 & I2C_SR1_RXNE_));

        /* get data */
        buf[i] = I2Cx->DR & I2C_DR_DR_Msk;
        
    }

    i2c7_stop(I2Cx);

}

