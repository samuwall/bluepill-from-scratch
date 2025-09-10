/* minblink.s */

.syntax unified
.thumb

.word 0x20005000
.word reset_handler + 1
.word nmi_handler + 1
.word hardfault_handler + 1

nmi_handler:
    b nmi_handler

hardfault_handler:
    b hardfault_handler

reset_handler:

    ldr  r0, =0x40021018    @ RCC_APB2ENR
    ldr  r1, [r0]
    orr  r1, #0x10          @ set bit 4 (IOPCEN)
    str  r1, [r0]

    ldr  r0, =0x40011004    @ GPIOC_CRH
    ldr  r1, [r0]
    orr  r1, #0x100000      @ set bit 20 (output 10MHz, open-drain)
    str  r1, [r0]

main_loop:
    ldr  r1, [r0, #8]       @ GPIOC_ODR 0x4001100C
    orr  r1, #0x2000        @ set bit 13 (turn off LED)
    str  r1, [r0, #8]
    ldr  r1, =500000        @ delay cntr = 500000

delay_1:
    subs r1, #1
    bne  delay_1

    ldr  r1, [r0, #8]       @ GPIOC_ODR 0x4001100C
    bic  r1, #0x2000        @ clear bit 13 (turn on LED)
    str  r1, [r0, #8]
    ldr  r1, =50000         @ reset delay cntr to 50000

delay_2:
    subs r1, #1
    bne  delay_2

    b    main_loop          @ repeat

