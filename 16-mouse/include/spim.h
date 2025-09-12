/**********************************************************************************
 ** file         : spim.h
 ** description  : generic spi operations (stm32f103)
 **
 **********************************************************************************/

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include "device.h"

uint16_t spi_transfer(SPI_T *SPIx, uint16_t data);

#endif
