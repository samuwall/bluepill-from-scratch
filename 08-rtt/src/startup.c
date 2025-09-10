/********************************************************************
 ** file         : startup.c
 ** description  : copy .data, zero .bss, branch to main()
 **
 **
 ********************************************************************/

#include <stdint.h>

extern int main(void);
extern uint8_t  _estack;
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void reset_handler(void) {
    
    uint32_t *src = &_etext;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    main();

    for(;;);
}

void default_handler(void) {
    for(;;);
}

/* core ISR declarations */
void nmi_handler(void)              __attribute__ ((weak, alias ("default_handler")));
void hardfault_handler(void)        __attribute__ ((weak, alias ("default_handler")));
void memmanage_fault_handler(void)  __attribute__ ((weak, alias ("default_handler")));
void busfault_handler(void)         __attribute__ ((weak, alias ("default_handler")));
void usagefault_handler(void)       __attribute__ ((weak, alias ("default_handler")));
void svc_handler(void)              __attribute__ ((weak, alias ("default_handler")));
void debugmon_handler(void)         __attribute__ ((weak, alias ("default_handler")));
void pendsv_handler(void)           __attribute__ ((weak, alias ("default_handler")));
void systick_handler(void)          __attribute__ ((weak, alias ("default_handler")));

/* stm32 ISR declarations */
void wwdg_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void pvd_isr(void)                  __attribute__ ((weak, alias ("default_handler")));
void tamper_isr(void)               __attribute__ ((weak, alias ("default_handler")));
void rtc_isr(void)                  __attribute__ ((weak, alias ("default_handler")));
void flash_isr(void)                __attribute__ ((weak, alias ("default_handler")));
void rcc_isr(void)                  __attribute__ ((weak, alias ("default_handler")));
void exti0_isr(void)                __attribute__ ((weak, alias ("default_handler")));
void exti1_isr(void)                __attribute__ ((weak, alias ("default_handler")));
void exti2_isr(void)                __attribute__ ((weak, alias ("default_handler")));
void exti3_isr(void)                __attribute__ ((weak, alias ("default_handler")));
void exti4_isr(void)                __attribute__ ((weak, alias ("default_handler")));
void dma1_channel1_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void dma1_channel2_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void dma1_channel3_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void dma1_channel4_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void dma1_channel5_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void dma1_channel6_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void dma1_channel7_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void adc1_2_isr(void)               __attribute__ ((weak, alias ("default_handler")));
void usb_hp_can_tx_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void usb_lp_can_rx0_isr(void)       __attribute__ ((weak, alias ("default_handler")));
void can_rx1_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void can_sce_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void exti9_5_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void tim1_brk_isr(void)             __attribute__ ((weak, alias ("default_handler")));
void tim1_up_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void tim1_trg_com_isr(void)         __attribute__ ((weak, alias ("default_handler")));
void tim1_cc_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void tim2_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void tim3_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void tim4_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void i2c1_ev_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void i2c1_er_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void i2c2_ev_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void i2c2_er_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void spi1_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void spi2_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void usart1_isr(void)               __attribute__ ((weak, alias ("default_handler")));
void usart2_isr(void)               __attribute__ ((weak, alias ("default_handler")));
void usart3_isr(void)               __attribute__ ((weak, alias ("default_handler")));
void exti15_10_isr(void)            __attribute__ ((weak, alias ("default_handler")));
void rtc_alarm_isr(void)            __attribute__ ((weak, alias ("default_handler")));
void usb_wakeup_isr(void)           __attribute__ ((weak, alias ("default_handler")));
void tim8_brk_isr(void)             __attribute__ ((weak, alias ("default_handler")));
void tim8_up_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void tim8_trg_com_isr(void)         __attribute__ ((weak, alias ("default_handler")));
void tim8_cc_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void adc3_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void fsmc_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void sdio_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void tim5_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void spi3_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void uart4_isr(void)                __attribute__ ((weak, alias ("default_handler")));
void uart5_isr(void)                __attribute__ ((weak, alias ("default_handler")));
void tim6_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void tim7_isr(void)                 __attribute__ ((weak, alias ("default_handler")));
void dma2_channel1_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void dma2_channel2_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void dma2_channel3_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void dma2_channel4_5_isr(void)      __attribute__ ((weak, alias ("default_handler")));
void dma2_channel5_isr(void)        __attribute__ ((weak, alias ("default_handler")));
void eth_isr(void)                  __attribute__ ((weak, alias ("default_handler")));
void eth_wkup_isr(void)             __attribute__ ((weak, alias ("default_handler")));
void can2_tx_isr(void)              __attribute__ ((weak, alias ("default_handler")));
void can2_rx0_isr(void)             __attribute__ ((weak, alias ("default_handler")));
void can2_rx1_isr(void)             __attribute__ ((weak, alias ("default_handler")));
void can2_sce_isr(void)             __attribute__ ((weak, alias ("default_handler")));
void otg_fs_isr(void)               __attribute__ ((weak, alias ("default_handler")));

const uint32_t vector_table[]
__attribute__ ((used, section(".vectors"))) = {
    (uint32_t) &_estack,
    (uint32_t) reset_handler,
    (uint32_t) nmi_handler,
    (uint32_t) hardfault_handler,
    (uint32_t) memmanage_fault_handler,
    (uint32_t) busfault_handler,
    (uint32_t) usagefault_handler,
    (uint32_t) 0,
    (uint32_t) 0,
    (uint32_t) 0,
    (uint32_t) 0,
    (uint32_t) svc_handler,
    (uint32_t) debugmon_handler,
    (uint32_t) 0,
    (uint32_t) pendsv_handler,
    (uint32_t) systick_handler,

    /* external interrupts, IRQ0..IRQ67 */
    (uint32_t) wwdg_isr,
    (uint32_t) pvd_isr,
    (uint32_t) tamper_isr,
    (uint32_t) rtc_isr,
    (uint32_t) flash_isr,
    (uint32_t) rcc_isr,
    (uint32_t) exti0_isr,
    (uint32_t) exti1_isr,
    (uint32_t) exti2_isr,
    (uint32_t) exti3_isr,
    (uint32_t) exti4_isr,
    (uint32_t) dma1_channel1_isr,
    (uint32_t) dma1_channel2_isr,
    (uint32_t) dma1_channel3_isr,
    (uint32_t) dma1_channel4_isr,
    (uint32_t) dma1_channel5_isr,
    (uint32_t) dma1_channel6_isr,
    (uint32_t) dma1_channel7_isr,
    (uint32_t) adc1_2_isr,
    (uint32_t) usb_hp_can_tx_isr,
    (uint32_t) usb_lp_can_rx0_isr,
    (uint32_t) can_rx1_isr,
    (uint32_t) can_sce_isr,
    (uint32_t) exti9_5_isr,
    (uint32_t) tim1_brk_isr,
    (uint32_t) tim1_up_isr,
    (uint32_t) tim1_trg_com_isr,
    (uint32_t) tim1_cc_isr,
    (uint32_t) tim2_isr,
    (uint32_t) tim3_isr,
    (uint32_t) tim4_isr,
    (uint32_t) i2c1_ev_isr,
    (uint32_t) i2c1_er_isr,
    (uint32_t) i2c2_ev_isr,
    (uint32_t) i2c2_er_isr,
    (uint32_t) spi1_isr,
    (uint32_t) spi2_isr,
    (uint32_t) usart1_isr,
    (uint32_t) usart2_isr,
    (uint32_t) usart3_isr,
    (uint32_t) exti15_10_isr,
    (uint32_t) rtc_alarm_isr,
    (uint32_t) usb_wakeup_isr,
    (uint32_t) tim8_brk_isr,
    (uint32_t) tim8_up_isr,
    (uint32_t) tim8_trg_com_isr,
    (uint32_t) tim8_cc_isr,
    (uint32_t) adc3_isr,
    (uint32_t) fsmc_isr,
    (uint32_t) sdio_isr,
    (uint32_t) tim5_isr,
    (uint32_t) spi3_isr,
    (uint32_t) uart4_isr,
    (uint32_t) uart5_isr,
    (uint32_t) tim6_isr,
    (uint32_t) tim7_isr,
    (uint32_t) dma2_channel1_isr,
    (uint32_t) dma2_channel2_isr,
    (uint32_t) dma2_channel3_isr,
    (uint32_t) dma2_channel4_5_isr,
    (uint32_t) dma2_channel5_isr,
    (uint32_t) eth_isr,
    (uint32_t) eth_wkup_isr,
    (uint32_t) can2_tx_isr,
    (uint32_t) can2_rx0_isr,
    (uint32_t) can2_rx1_isr,
    (uint32_t) can2_sce_isr,
    (uint32_t) otg_fs_isr
};
