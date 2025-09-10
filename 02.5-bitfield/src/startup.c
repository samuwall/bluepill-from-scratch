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

void nmi_handler(void) {
   for(;;);
}

void hardfault_handler(void) {
    for (;;);
}

const uint32_t vector_table[]
__attribute__((used, section(".vectors"))) = {
    (uint32_t) &_estack,
    (uint32_t) reset_handler,
    (uint32_t) nmi_handler,
    (uint32_t) hardfault_handler,
};

