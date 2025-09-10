/**********************************************************************************
 ** file         : i2c7.h
 ** description  : 
 **
 **********************************************************************************/

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stddef.h>
#include "device.h"

#define ADDR_LSB_WRITE 0
#define ADDR_LSB_READ  1
#define I2C_STOP   1
#define I2C_NOSTOP 0

void i2c7_write(I2C_T *I2Cx, uint8_t slave_addr, const uint8_t *data, size_t n, int stop);
void i2c7_read(I2C_T *I2Cx, uint8_t slave_addr, uint8_t *buf, size_t n);
void i2c7_stop(I2C_T *I2Cx);

#endif