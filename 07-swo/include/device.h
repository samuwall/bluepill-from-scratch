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
#define IO16 volatile uint16_t
#define IO8  volatile uint8_t

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
    IO32 SR;
    IO32 DR;
    IO32 BRR;
    IO32 CR1;
    IO32 CR2;
    IO32 CR3;
    IO32 GTPR;
} USART_T;

typedef struct {
    volatile union {
        IO8 u8;
        IO16 u16;
        IO32 u32;
    } PORT[32];
    IO32 RESERVED0[864];
    IO32 TER;
    IO32 RESERVED1[15];
    IO32 TPR;
    IO32 RESERVED2[15];
    IO32 TCR;
    IO32 RESERVED3[29];
    IO32 IWR;
    IO32 IRR;
    IO32 IMCR;
    IO32 RESERVED4[43];
    IO32 LAR;
    IO32 LSR;
} ITM_T;

typedef struct {
    IO32 CSR;
    IO32 RVR;
    IO32 CVR;
    IO32 CALIB;
} STK_T;

typedef struct {
    IO32 ISER[8];
    IO32 RESERVED0[24];
    IO32 ICER[8];
    IO32 RESERVED1[24];
    IO32 ISPR[8];
    IO32 RESERVED2[24];
    IO32 ICPR[8];
    IO32 RESERVED3[24];
    IO32 IABR[8];
    IO32 RESERVED4[56];
    IO8  IPR[240];
    IO32 RESERVED5[644];
    IO32 STIR;
}  NVIC_T;

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

#define GPIO_CRL_CNFMODE0_Shft      0U
#define GPIO_CRL_CNFMODE1_Shft      4U
#define GPIO_CRL_CNFMODE2_Shft      8U
#define GPIO_CRL_CNFMODE3_Shft      12U
#define GPIO_CRL_CNFMODE4_Shft      16U
#define GPIO_CRL_CNFMODE5_Shft      20U
#define GPIO_CRL_CNFMODE6_Shft      24U
#define GPIO_CRL_CNFMODE7_Shft      28U
#define GPIO_CRH_CNFMODE8_Shft      0U
#define GPIO_CRH_CNFMODE9_Shft      4U
#define GPIO_CRH_CNFMODE10_Shft     8U
#define GPIO_CRH_CNFMODE11_Shft     12U
#define GPIO_CRH_CNFMODE12_Shft     16U
#define GPIO_CRH_CNFMODE13_Shft     20U
#define GPIO_CRH_CNFMODE14_Shft     24U
#define GPIO_CRH_CNFMODE15_Shft     28U

#define USART_DR_DR_Shft            0U
#define USART_CR1_MODE_Shft         2U      /* USART_CR1_RE | USART_CR1_TE */
#define USART_CR1_PARITY_Shft       9U      /* USART_CR1_PS | USART_CR1_PCE */
#define USART_CR1_M_Shft            12U
#define USART_CR2_STOP_Shft         12U
#define USART_CR3_FLOWCNTRL_Shft    8U      /* USART_CR3_RTSE | USART_CR3_CTSE */

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

#define GPIO_CRL_CNFMODE0_Msk       (0b1111 << GPIO_CRL_CNFMODE0_Shft)
#define GPIO_CRL_CNFMODE1_Msk       (0b1111 << GPIO_CRL_CNFMODE1_Shft)
#define GPIO_CRL_CNFMODE2_Msk       (0b1111 << GPIO_CRL_CNFMODE2_Shft)
#define GPIO_CRL_CNFMODE3_Msk       (0b1111 << GPIO_CRL_CNFMODE3_Shft)
#define GPIO_CRL_CNFMODE4_Msk       (0b1111 << GPIO_CRL_CNFMODE4_Shft)
#define GPIO_CRL_CNFMODE5_Msk       (0b1111 << GPIO_CRL_CNFMODE5_Shft)
#define GPIO_CRL_CNFMODE6_Msk       (0b1111 << GPIO_CRL_CNFMODE6_Shft)
#define GPIO_CRL_CNFMODE7_Msk       (0b1111 << GPIO_CRL_CNFMODE7_Shft)
#define GPIO_CRH_CNFMODE8_Msk       (0b1111 << GPIO_CRH_CNFMODE8_Shft)
#define GPIO_CRH_CNFMODE9_Msk       (0b1111 << GPIO_CRH_CNFMODE9_Shft)
#define GPIO_CRH_CNFMODE10_Msk      (0b1111 << GPIO_CRH_CNFMODE10_Shft)
#define GPIO_CRH_CNFMODE11_Msk      (0b1111 << GPIO_CRH_CNFMODE11_Shft)
#define GPIO_CRH_CNFMODE12_Msk      (0b1111 << GPIO_CRH_CNFMODE12_Shft)
#define GPIO_CRH_CNFMODE13_Msk      (0b1111 << GPIO_CRH_CNFMODE13_Shft)
#define GPIO_CRH_CNFMODE14_Msk      (0b1111 << GPIO_CRH_CNFMODE14_Shft)
#define GPIO_CRH_CNFMODE15_Msk      (0b1111 << GPIO_CRH_CNFMODE15_Shft)

#define USART_DR_DR_Msk             (0b11111111 << USART_DR_DR_Shft)
#define USART_CR1_MODE_Msk          (0b11   << USART_CR1_MODE_Shft)
#define USART_CR1_PARITY_Msk        (0b11   << USART_CR1_PARITY_Shft)
#define USART_CR1_M_Msk             (0b1    << USART_CR1_M_Shft)
#define USART_CR2_STOP_Msk          (0b11   << USART_CR2_STOP_Shft)
#define USART_CR3_FLOWCNTRL_Msk     (0b11   << USART_CR3_FLOWCNTRL_Shft)

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

#define RCC_APB1ENR_TIM2EN_         (1 << 0)
#define RCC_APB1ENR_TIM3EN_         (1 << 1)
#define RCC_APB1ENR_TIM4EN_         (1 << 2)
#define RCC_APB1ENR_TIM5EN_         (1 << 3)
#define RCC_APB1ENR_TIM6EN_         (1 << 4)
#define RCC_APB1ENR_TIM7EN_         (1 << 5)
#define RCC_APB1ENR_WWDGEN_         (1 << 11)
#define RCC_APB1ENR_SPI2EN_         (1 << 14)
#define RCC_APB1ENR_SPI3EN_         (1 << 15)
#define RCC_APB1ENR_USART2EN_       (1 << 17)
#define RCC_APB1ENR_USART3EN_       (1 << 18)
#define RCC_APB1ENR_UART4EN_        (1 << 19)
#define RCC_APB1ENR_UART5EN_        (1 << 20)
#define RCC_APB1ENR_I2C1EN_         (1 << 21)
#define RCC_APB1ENR_I2C2EN_         (1 << 22)
#define RCC_APB1ENR_USBEN_          (1 << 23)
#define RCC_APB1ENR_CANEN_          (1 << 25)
#define RCC_APB1ENR_BKPEN_          (1 << 27)
#define RCC_APB1ENR_PWREN_          (1 << 28)
#define RCC_APB1ENR_DACEN_          (1 << 29)

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

/* ---  USART -------------------------------------------------------------- */

#define USART_SR_TXE_               (1 << 7)    /* 1 = TDR is empty, data already in shift register */
#define USART_SR_RXNE_              (1 << 5)    /* 1 = RDR is not empty, data received and ready to be read */

#define USART_CR1_MODE_TX           (0b10 << USART_CR1_MODE_Shft)
#define USART_CR1_MODE_RX           (0b01 << USART_CR1_MODE_Shft)
#define USART_CR1_MODE_TX_RX        (0b11 << USART_CR1_MODE_Shft)
#define USART_CR1_PARITY_NONE       (0b00 << USART_CR1_PARITY_Shft)
#define USART_CR1_PARITY_EVEN       (0b10 << USART_CR1_PARITY_Shft)
#define USART_CR1_M_8BITS           (0b0  << USART_CR1_M_Shft)
#define USART_CR1_M_9BITS           (0b1  << USART_CR1_M_Shft)
#define USART_CR1_UE_               (1 << 13)   /* 1 = USART enable */
#define USART_CR1_TXEIE_            (1 << 7)    /* 1 = A USART interrupt is generated whenever TXE=1 in the USART_SR register */
#define USART_CR1_RXNEIE_           (1 << 5)    /* 1 = A USART interrupt is generated whenever RXNE=1 or ORE=1 in the USART_SR register */

#define USART_CR2_STOP_1BIT         (0b00 << USART_CR2_STOP_Shft)
#define USART_CR2_STOP_2BIT         (0b10 << USART_CR2_STOP_Shft)

#define USART_CR3_FLOWCNTRL_NONE    (0b00 << USART_CR3_FLOWCNTRL_Shft)

/* ---  ITM ---------------------------------------------------------------- */

#define ITM_STIM_FIFONOTFULL_        (1 << 0)

/* ---  STK ---------------------------------------------------------------- */

#define STK_CSR_CLKSOURCE_AHBDIV    (1 << 2)    /* 0 = AHB/8, 1 = AHB */
#define STK_CSR_TICKINT_            (1 << 1)    /* 1 = enable systick interrupt */
#define STK_CSR_ENABLE_             (1 << 0)    /* 1 = enable systick counter */

/* --- NVIC IRQ Numbers -------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

#define NVIC_WWDG_IRQ               0
#define NVIC_PVD_IRQ                1
#define NVIC_TAMPER_IRQ             2
#define NVIC_RTC_IRQ                3  
#define NVIC_FLASH_IRQ              4
#define NVIC_RCC_IRQ                5
#define NVIC_EXTI0_IRQ              6
#define NVIC_EXTI1_IRQ              7
#define NVIC_EXTI2_IRQ              8
#define NVIC_EXTI3_IRQ              9
#define NVIC_EXTI4_IRQ              10
#define NVIC_DMA1_CHANNEL1_IRQ      11
#define NVIC_DMA1_CHANNEL2_IRQ      12
#define NVIC_DMA1_CHANNEL3_IRQ      13
#define NVIC_DMA1_CHANNEL4_IRQ      14
#define NVIC_DMA1_CHANNEL5_IRQ      15
#define NVIC_DMA1_CHANNEL6_IRQ      16
#define NVIC_DMA1_CHANNEL7_IRQ      17
#define NVIC_ADC1_2_IRQ             18
#define NVIC_USB_HP_CAN_TX_IRQ      19
#define NVIC_USB_LP_CAN_RX0_IRQ     20
#define NVIC_CAN_RX1_IRQ            21
#define NVIC_CAN_SCE_IRQ            22
#define NVIC_EXTI9_5_IRQ            23
#define NVIC_TIM1_BRK_IRQ           24
#define NVIC_TIM1_UP_IRQ            25
#define NVIC_TIM1_TRG_COM_IRQ       26
#define NVIC_TIM1_CC_IRQ            27
#define NVIC_TIM2_IRQ               28
#define NVIC_TIM3_IRQ               29
#define NVIC_TIM4_IRQ               30
#define NVIC_I2C1_EV_IRQ            31
#define NVIC_I2C1_ER_IRQ            32
#define NVIC_I2C2_EV_IRQ            33
#define NVIC_I2C2_ER_IRQ            34
#define NVIC_SPI1_IRQ               35
#define NVIC_SPI2_IRQ               36
#define NVIC_USART1_IRQ             37
#define NVIC_USART2_IRQ             38
#define NVIC_USART3_IRQ             39
#define NVIC_EXTI15_10_IRQ          40
#define NVIC_RTC_ALARM_IRQ          41
#define NVIC_USB_WAKEUP_IRQ         42
#define NVIC_TIM8_BRK_IRQ           43
#define NVIC_TIM8_UP_IRQ            44
#define NVIC_TIM8_TRG_COM_IRQ       45
#define NVIC_TIM8_CC_IRQ            46
#define NVIC_ADC3_IRQ               47
#define NVIC_FSMC_IRQ               48
#define NVIC_SDIO_IRQ               49
#define NVIC_TIM5_IRQ               50
#define NVIC_SPI3_IRQ               51
#define NVIC_UART4_IRQ              52
#define NVIC_UART5_IRQ              53
#define NVIC_TIM6_IRQ               54
#define NVIC_TIM7_IRQ               55
#define NVIC_DMA2_CHANNEL1_IRQ      56
#define NVIC_DMA2_CHANNEL2_IRQ      57
#define NVIC_DMA2_CHANNEL3_IRQ      58
#define NVIC_DMA2_CHANNEL4_5_IRQ    59
#define NVIC_DMA2_CHANNEL5_IRQ      60
#define NVIC_ETH_IRQ                61
#define NVIC_ETH_WKUP_IRQ           62
#define NVIC_CAN2_TX_IRQ            63
#define NVIC_CAN2_RX0_IRQ           64
#define NVIC_CAN2_RX1_IRQ           65
#define NVIC_CAN2_SCE_IRQ           66
#define NVIC_OTG_FS_IRQ             67

/* --- POINTERS ---------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

#define RCC         ((RCC_T *)      0x40021000)
#define GPIOA       ((GPIO_T *)     0x40010800)
#define GPIOC       ((GPIO_T *)     0x40011000)
#define FLASH_ACR   (*(IO32 *)      0x40022000)
#define USART2      ((USART_T *)    0x40004400)
#define ITM         ((ITM_T *)      0xE0000000)
#define STK         ((STK_T *)      0xE000E010)
#define NVIC        ((NVIC_T *)     0xE000E100)

#endif