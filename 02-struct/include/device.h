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

/* compiler calculates offsets automatically based on the 32-bit member types equaling 4 bytes each,
 * and orders them in memory in the same order in which they are defined
 */
typedef struct {        /* offsets */
    IO32 CR;            /* 0x00  */ 
    IO32 CFGR;          /* 0x04  */
    IO32 CIR;           /* 0x08  */
    IO32 APB2RSTR;      /* etc.. */
    IO32 APB1RSTR;
    IO32 AHBENR;
    IO32 APB2ENR;
    IO32 APB1ENR;
    IO32 BDCR;
} RCC_T;

typedef struct {
    IO32 CRL;
    IO32 CRH;
    IO32 IDR;
    IO32 ODR;
    IO32 BSRR;
    IO32 BRR;
    IO32 LCKR;
} GPIO_T;

/* --- BITFIELDS --------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

/* --- SHIFT VALUES -------------------------------------------------------- */

#define GPIO_CRH_CNFMODE8_Shft      0U
#define GPIO_CRH_CNFMODE9_Shft      4U
#define GPIO_CRH_CNFMODE10_Shft     8U
#define GPIO_CRH_CNFMODE11_Shft     12U
#define GPIO_CRH_CNFMODE12_Shft     16U
#define GPIO_CRH_CNFMODE13_Shft     20U
#define GPIO_CRH_CNFMODE14_Shft     24U
#define GPIO_CRH_CNFMODE15_Shft     28U

/* --- BITMASKS -------------------------------------------------------- */

#define GPIO_CRH_CNFMODE8_Msk       (0b1111 << GPIO_CRH_CNFMODE8_Shft)
#define GPIO_CRH_CNFMODE9_Msk       (0b1111 << GPIO_CRH_CNFMODE9_Shft)
#define GPIO_CRH_CNFMODE10_Msk      (0b1111 << GPIO_CRH_CNFMODE10_Shft)
#define GPIO_CRH_CNFMODE11_Msk      (0b1111 << GPIO_CRH_CNFMODE11_Shft)
#define GPIO_CRH_CNFMODE12_Msk      (0b1111 << GPIO_CRH_CNFMODE12_Shft)
#define GPIO_CRH_CNFMODE13_Msk      (0b1111 << GPIO_CRH_CNFMODE13_Shft)
#define GPIO_CRH_CNFMODE14_Msk      (0b1111 << GPIO_CRH_CNFMODE14_Shft)
#define GPIO_CRH_CNFMODE15_Msk      (0b1111 << GPIO_CRH_CNFMODE15_Shft)

/* --- BITFIELD VALUES --------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

#define RCC_APB2ENR_AFIOEN_         (1 << 0)    /* the idea with the trailing '_' is that these values are also their own fields */
#define RCC_APB2ENR_IOPAEN_         (1 << 2)
#define RCC_APB2ENR_IOPBEN_         (1 << 3)
#define RCC_APB2ENR_IOPCEN_         (1 << 4)
#define RCC_APB2ENR_IOPDEN_         (1 << 5)
#define RCC_APB2ENR_IOPEEN_         (1 << 6)
#define RCC_APB2ENR_IOPFEN_         (1 << 7)
#define RCC_APB2ENR_IOPGEN_         (1 << 8)
#define RCC_APB2ENR_ADC1EN_         (1 << 9)
#define RCC_APB2ENR_ADC2EN_         (1 << 10)
#define RCC_APB2ENR_TIM1EN_         (1 << 11)
#define RCC_APB2ENR_SPI1EN_         (1 << 12)
#define RCC_APB2ENR_TIM8EN_         (1 << 13)
#define RCC_APB2ENR_USART1EN_       (1 << 14)
#define RCC_APB2ENR_ADC3EN_         (1 << 15)
#define RCC_APB2ENR_TIM15EN_        (1 << 16)
#define RCC_APB2ENR_TIM16EN_        (1 << 17)
#define RCC_APB2ENR_TIM17EN_        (1 << 18)

#define GPIO_CNFMODE_INPUT_ANALOG               0b0000
#define GPIO_CNFMODE_INPUT_FLOAT                0b0100
#define GPIO_CNFMODE_INPUT_PUPD                 0b1000
#define GPIO_CNFMODE_OUTPUT_GP_PUSHPULL_10MHZ   0b0001
#define GPIO_CNFMODE_OUTPUT_GP_PUSHPULL_2MHZ    0b0010
#define GPIO_CNFMODE_OUTPUT_GP_PUSHPULL_50MHZ   0b0011
#define GPIO_CNFMODE_OUTPUT_GP_OPENDRAIN_10MHZ  0b0101
#define GPIO_CNFMODE_OUTPUT_GP_OPENDRAIN_2MHZ   0b0110
#define GPIO_CNFMODE_OUTPUT_GP_OPENDRAIN_50MHZ  0b0111
#define GPIO_CNFMODE_OUTPUT_AF_PUSHPULL_10MHZ   0b1001
#define GPIO_CNFMODE_OUTPUT_AF_PUSHPULL_2MHZ    0b1010
#define GPIO_CNFMODE_OUTPUT_AF_PUSHPULL_50MHZ   0b1011
#define GPIO_CNFMODE_OUTPUT_AF_OPENDRAIN_10MHZ  0b1101
#define GPIO_CNFMODE_OUTPUT_AF_OPENDRAIN_2MHZ   0b1110
#define GPIO_CNFMODE_OUTPUT_AF_OPENDRAIN_50MHZ  0b1111

#define GPIO0                       (1 << 0)
#define GPIO1                       (1 << 1)
#define GPIO2                       (1 << 2)
#define GPIO3                       (1 << 3)
#define GPIO4                       (1 << 4)
#define GPIO5                       (1 << 5)
#define GPIO6                       (1 << 6)
#define GPIO7                       (1 << 7)
#define GPIO8                       (1 << 8)
#define GPIO9                       (1 << 9)
#define GPIO10                      (1 << 10)
#define GPIO11                      (1 << 11)
#define GPIO12                      (1 << 12)
#define GPIO13                      (1 << 13)
#define GPIO14                      (1 << 14)
#define GPIO15                      (1 << 15)

/* --- POINTERS ---------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

#define RCC     ((RCC_T *)  0x40021000)
#define GPIOC   ((GPIO_T *) 0x40011000)

#endif