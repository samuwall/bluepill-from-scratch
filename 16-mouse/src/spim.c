/**********************************************************************************
 ** file            : spim.c
 ** description     : generic spi master operations (stm32f103)
 **
 **********************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "device.h"
#include "spim.h"

uint16_t spi_transfer(SPI_T *SPIx, uint16_t data) {

    while (!(SPIx->SR & SPI_SR_TXE_));

    SPIx->DR = data;

    while (!(SPIx->SR & SPI_SR_RXNE_));

    return SPIx->DR;
}
