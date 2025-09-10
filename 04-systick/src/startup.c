/********************************************************************
 ** file         : startup.c
 ** description  : copy .data, zero .bss, branch to main()
 **
 **
 ********************************************************************/

#include <stdint.h>

extern int main(void);
extern uint32_t _estack;
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void reset_handler(void);
void default_handler(void);
void nmi_handler(void)              __attribute__ ((weak, alias ("default_handler")));
void hardfault_handler(void)        __attribute__ ((weak, alias ("default_handler")));
void memmanage_fault_handler(void)  __attribute__ ((weak, alias ("default_handler")));
void busfault_handler(void)         __attribute__ ((weak, alias ("default_handler")));
void usagefault_handler(void)       __attribute__ ((weak, alias ("default_handler")));
void svc_handler(void)              __attribute__ ((weak, alias ("default_handler")));
void debugmon_handler(void)         __attribute__ ((weak, alias ("default_handler")));
void pendsv_handler(void)           __attribute__ ((weak, alias ("default_handler")));
void systick_handler(void)          __attribute__ ((weak, alias ("default_handler")));

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
    (uint32_t) systick_handler
};

