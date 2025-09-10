/********************************************************************
 ** file         : device.h
 ** description  : structs and macros for core/device peripheral access
 **
 ** device       : STM32F103
 **
 ********************************************************************/

#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

#define IO32 volatile uint32_t

/* --- PERIPHERAL STRUCTS ------------------------------------------------------------ */
/* ----------------------------------------------------------------------------------- */

/* 
 * unions allow for both the 32-bit reg and bitfields to represent the same address 
 */

typedef struct {
    IO32 CR;
    IO32 CFGR;
    IO32 CIR;
    IO32 APB2RSTR;
    IO32 APB1RSTR;
    IO32 AHBENR;
    union {
        IO32 reg;
        struct {
            IO32 AFIOEN:    1;
            IO32 RESERVED0: 1;
            IO32 IOPAEN:    1;
            IO32 IOPBEN:    1;
            IO32 IOPCEN:    1;
            IO32 IOPDEN:    1;
            IO32 IOPEEN:    1;
            IO32 IOPFEN:    1;
            IO32 IOPGEN:    1;
            IO32 ADC1EN:    1;
            IO32 ADC2EN:    1;
            IO32 TIM1EN:    1;
            IO32 SPI1EN:    1;
            IO32 TIM8EN:    1;
            IO32 USART1EN:  1;
            IO32 ADC3EN:    1;
            IO32 RESERVED1: 3;
            IO32 TIM9EN:    1;
            IO32 TIM10EN:   1;
            IO32 TIM11EN:   1;
            IO32 RESERVED2: 10;
        } bit;
    } APB2ENR;
    IO32 APB1ENR;
    IO32 BDCR;
} RCC_T;

typedef struct {
    IO32 CRL;
    union {
        IO32 reg;
        struct {
            IO32 CNF_MODE8:  4;
            IO32 CNF_MODE9:  4;
            IO32 CNF_MODE10: 4;
            IO32 CNF_MODE11: 4;
            IO32 CNF_MODE12: 4;
            IO32 CNF_MODE13: 4;
            IO32 CNF_MODE14: 4;
            IO32 CNF_MODE15: 4;

        } bit;
    } CRH;
    IO32 IDR;
    union {
        IO32 reg;
        struct {
            IO32 ODR0:  1;
            IO32 ODR1:  1;
            IO32 ODR2:  1;
            IO32 ODR3:  1;
            IO32 ODR4:  1;
            IO32 ODR5:  1;
            IO32 ODR6:  1;
            IO32 ODR7:  1;
            IO32 ODR8:  1;
            IO32 ODR9:  1;
            IO32 ODR10: 1;
            IO32 ODR11: 1;
            IO32 ODR12: 1;
            IO32 ODR13: 1;
            IO32 ODR14: 1;
            IO32 ODR15: 1;
        } bit;
    } ODR;
    IO32 BSRR;
    IO32 BRR;
    IO32 LCKR;
} GPIO_T;

/* --- BITFIELD VALUES --------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

#define GPIO13  (1 << 13)
#define GPIO_CNFMODE_OUTPUT_GP_OPENDRAIN_2MHZ  0b0110

/* --- POINTERS ---------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

#define RCC     ((RCC_T *)  0x40021000)
#define GPIOC   ((GPIO_T *) 0x40011000)

#endif