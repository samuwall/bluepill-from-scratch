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

typedef struct {
    IO32 CSR;
    IO32 RVR;
    IO32 CVR;
    IO32 CALIB;
} STK_T;

/* --- BITFIELDS --------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

/* --- SHIFT VALUES -------------------------------------------------------- */

#define FLASH_ACR_LATENCY_Shft      0U

#define RCC_CFGR_SW_Shft            0U
#define RCC_CFGR_SWS_Shft           2U
#define RCC_CFGR_HPRE_Shft          4U
#define RCC_CFGR_PPRE1_Shft         8U
#define RCC_CFGR_PPRE2_Shft         11U
#define RCC_CFGR_ADCPRE_Shft        14U
#define RCC_CFGR_PLLSRC_Shft        16U
#define RCC_CFGR_PLLXTPRE_Shft      17U
#define RCC_CFGR_PLLMUL_Shft        18U
#define RCC_CFGR_USBPRE_Shft        22U
#define RCC_CFGR_MCO_Shft           24U

#define GPIO_CRH_CNFMODE8_Shft      0U
#define GPIO_CRH_CNFMODE9_Shft      4U
#define GPIO_CRH_CNFMODE10_Shft     8U
#define GPIO_CRH_CNFMODE11_Shft     12U
#define GPIO_CRH_CNFMODE12_Shft     16U
#define GPIO_CRH_CNFMODE13_Shft     20U
#define GPIO_CRH_CNFMODE14_Shft     24U
#define GPIO_CRH_CNFMODE15_Shft     28U

#define STK_RVR_RELOAD_Shft         0U
#define STK_CSR_CLKSOURCE_Shft      2U

/* --- BITMASKS -------------------------------------------------------- */

#define FLASH_ACR_LATENCY_Msk       (0b111  << FLASH_ACR_LATENCY_Shft)

#define RCC_CFGR_SW_Msk             (0b11   << RCC_CFGR_SW_Shft)
#define RCC_CFGR_SWS_Msk            (0b11   << RCC_CFGR_SWS_Shft)
#define RCC_CFGR_HPRE_Msk           (0b1111 << RCC_CFGR_HPRE_Shft)
#define RCC_CFGR_PPRE1_Msk          (0b111  << RCC_CFGR_PPRE1_Shft)
#define RCC_CFGR_PPRE2_Msk          (0b111  << RCC_CFGR_PPRE2_Shft)
#define RCC_CFGR_ADCPRE_Msk         (0b11   << RCC_CFGR_ADCPRE_Shft)
#define RCC_CFGR_PLLSRC_Msk         (0b1    << RCC_CFGR_PLLSRC_Shft)
#define RCC_CFGR_PLLXTPRE_Msk       (0b1    << RCC_CFGR_PLLXTPRE_Shft)
#define RCC_CFGR_PLLMUL_Msk         (0b1111 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_USBPRE_Msk         (0b1    << RCC_CFGR_USBPRE_Shft)
#define RCC_CFGR_MCO_Msk            (0b111  << RCC_CFGR_MCO_Shft)

#define GPIO_CRH_CNFMODE8_Msk       (0b1111 << GPIO_CRH_CNFMODE8_Shft)
#define GPIO_CRH_CNFMODE9_Msk       (0b1111 << GPIO_CRH_CNFMODE9_Shft)
#define GPIO_CRH_CNFMODE10_Msk      (0b1111 << GPIO_CRH_CNFMODE10_Shft)
#define GPIO_CRH_CNFMODE11_Msk      (0b1111 << GPIO_CRH_CNFMODE11_Shft)
#define GPIO_CRH_CNFMODE12_Msk      (0b1111 << GPIO_CRH_CNFMODE12_Shft)
#define GPIO_CRH_CNFMODE13_Msk      (0b1111 << GPIO_CRH_CNFMODE13_Shft)
#define GPIO_CRH_CNFMODE14_Msk      (0b1111 << GPIO_CRH_CNFMODE14_Shft)
#define GPIO_CRH_CNFMODE15_Msk      (0b1111 << GPIO_CRH_CNFMODE15_Shft)

#define STK_RVR_RELOAD_Msk          (0x00FFFFFF << STK_RVR_RELOAD_Shft)
#define STK_CSR_CLKSOURCE_Msk       (0b1    << STK_CSR_CLKSOURCE_Shft)

/* --- BITFIELD VALUES --------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

/* ---  FLASH -------------------------------------------------------------- */

#define FLASH_ACR_LATENCY_0WAIT     (0b000 << FLASH_ACR_LATENCY_Shft)
#define FLASH_ACR_LATENCY_1WAIT     (0b001 << FLASH_ACR_LATENCY_Shft)
#define FLASH_ACR_LATENCY_2WAIT     (0b010 << FLASH_ACR_LATENCY_Shft)

/* --- RCC ----------------------------------------------------------------- */

#define RCC_CR_HSION_               (1 << 0) 
#define RCC_CR_HSIRDY_              (1 << 1)
#define RCC_CR_HSEON_               (1 << 16)
#define RCC_CR_HSERDY_              (1 << 17)
#define RCC_CR_HSEBYP_              (1 << 18)
#define RCC_CR_CSSON_               (1 << 19)
#define RCC_CR_PLLON_               (1 << 24)
#define RCC_CR_PLLRDY_              (1 << 25)

#define RCC_CFGR_SW_HSICLK          (0b00 << RCC_CFGR_SW_Shft)
#define RCC_CFGR_SW_HSECLK          (0b01 << RCC_CFGR_SW_Shft)
#define RCC_CFGR_SW_PLLCLK          (0b10 << RCC_CFGR_SW_Shft)
#define RCC_CFGR_SWS_HSICLK         (0b00 << RCC_CFGR_SWS_Shft)
#define RCC_CFGR_SWS_HSECLK         (0b01 << RCC_CFGR_SWS_Shft)
#define RCC_CFGR_SWS_PLLCLK         (0b10 << RCC_CFGR_SWS_Shft)
#define RCC_CFGR_HPRE_NODIV         (0b0000 << RCC_CFGR_HPRE_Shft)
#define RCC_CFGR_HPRE_DIV2          (0b1000 << RCC_CFGR_HPRE_Shft)
#define RCC_CFGR_HPRE_DIV4          (0b1001 << RCC_CFGR_HPRE_Shft)
#define RCC_CFGR_HPRE_DIV8          (0b1010 << RCC_CFGR_HPRE_Shft)
#define RCC_CFGR_HPRE_DIV16         (0b1011 << RCC_CFGR_HPRE_Shft)
#define RCC_CFGR_HPRE_DIV64         (0b1100 << RCC_CFGR_HPRE_Shft)
#define RCC_CFGR_HPRE_DIV128        (0b1101 << RCC_CFGR_HPRE_Shft)
#define RCC_CFGR_HPRE_DIV256        (0b1110 << RCC_CFGR_HPRE_Shft)
#define RCC_CFGR_HPRE_DIV512        (0b1111 << RCC_CFGR_HPRE_Shft)
#define RCC_CFGR_PPRE1_NODIV        (0b000 << RCC_CFGR_PPRE1_Shft)
#define RCC_CFGR_PPRE1_DIV2         (0b100 << RCC_CFGR_PPRE1_Shft)
#define RCC_CFGR_PPRE1_DIV4         (0b101 << RCC_CFGR_PPRE1_Shft)
#define RCC_CFGR_PPRE1_DIV8         (0b110 << RCC_CFGR_PPRE1_Shft)
#define RCC_CFGR_PPRE1_DIV16        (0b111 << RCC_CFGR_PPRE1_Shft)
#define RCC_CFGR_PPRE2_NODIV        (0b000 << RCC_CFGR_PPRE2_Shft)
#define RCC_CFGR_PPRE2_DIV2         (0b100 << RCC_CFGR_PPRE2_Shft)
#define RCC_CFGR_PPRE2_DIV4         (0b101 << RCC_CFGR_PPRE2_Shft)
#define RCC_CFGR_PPRE2_DIV8         (0b110 << RCC_CFGR_PPRE2_Shft)
#define RCC_CFGR_PPRE2_DIV16        (0b111 << RCC_CFGR_PPRE2_Shft)
#define RCC_CFGR_ADCPRE_DIV2        (0b00 << RCC_CFGR_ADCPRE_Shft)
#define RCC_CFGR_ADCPRE_DIV4        (0b01 << RCC_CFGR_ADCPRE_Shft)
#define RCC_CFGR_ADCPRE_DIV6        (0b10 << RCC_CFGR_ADCPRE_Shft)
#define RCC_CFGR_ADCPRE_DIV8        (0b11 << RCC_CFGR_ADCPRE_Shft)
#define RCC_CFGR_PLLSRC_HSIDIV2     (0b0 << RCC_CFGR_PLLSRC_Shft)
#define RCC_CFGR_PLLSRC_HSE         (0b1 << RCC_CFGR_PLLSRC_Shft)
#define RCC_CFGR_PLLXTPRE_HSE       (0b0 << RCC_CFGR_PLLXTPRE_Shft)
#define RCC_CFGR_PLLXTPRE_HSEDIV2   (0b1 << RCC_CFGR_PLLXTPRE_Shft)
#define RCC_CFGR_PLLMUL_2           (0b0000 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_3           (0b0001 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_4           (0b0010 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_5           (0b0011 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_6           (0b0100 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_7           (0b0101 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_8           (0b0110 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_9           (0b0111 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_10          (0b1000 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_11          (0b1001 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_12          (0b1010 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_13          (0b1011 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_14          (0b1100 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_15          (0b1101 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_PLLMUL_16          (0b1111 << RCC_CFGR_PLLMUL_Shft)
#define RCC_CFGR_USBPRE_PLLDIV15    (0b0 << RCC_CFGR_USBPRE_Shft)
#define RCC_CFGR_USBPRE_PLLNODIV    (0b1 << RCC_CFGR_USBPRE_Shft)
#define RCC_CFGR_MCO_NOCLK          (0b000 << RCC_CFGR_MCO_Shft)
#define RCC_CFGR_MCO_SYSCLK         (0b100 << RCC_CFGR_MCO_Shft)
#define RCC_CFGR_MCO_HSI            (0b101 << RCC_CFGR_MCO_Shft)
#define RCC_CFGR_MCO_HSE            (0b110 << RCC_CFGR_MCO_Shft)
#define RCC_CFGR_MCO_PLLDIV2        (0b111 << RCC_CFGR_MCO_Shft)

#define RCC_APB2ENR_AFIOEN_         (1 << 0)
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

/* ---  GPIO --------------------------------------------------------------- */

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

/* ---  STK ---------------------------------------------------------------- */

#define STK_CSR_CLKSOURCE_AHBDIV    (1 << 2)    /* 0 = AHB/8, 1 = AHB */
#define STK_CSR_TICKINT_            (1 << 1)    /* 1 = enable systick interrupt */
#define STK_CSR_ENABLE_             (1 << 0)    /* 1 = enable systick counter */

/* --- POINTERS ---------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

#define RCC         ((RCC_T *)  0x40021000)
#define GPIOC       ((GPIO_T *) 0x40011000)
#define FLASH_ACR   (*(volatile uint32_t *) 0x40022000)
#define STK         ((STK_T *) 0xE000E010)

#endif