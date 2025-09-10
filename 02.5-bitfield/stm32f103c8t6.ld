/********************************************************************
 ** file         : stm32f103c8t6.ld
 ** description  : no libc, no c++, minimal linker script
 **
 **
 ********************************************************************/

MEMORY
{
    RAM   (rwx) : ORIGIN = 0x20000000,  LENGTH = 20K
    FLASH (rx)  : ORIGIN = 0x8000000,   LENGTH = 64K
}

_estack = ORIGIN(RAM) + LENGTH(RAM); 

SECTIONS
{
    .text :
    {
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
    } > FLASH

    _etext = .;

    .data :
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM AT> FLASH

    .bss :
    {
        _sbss = .;
        *(.bss*)
        _ebss = .;
    } > RAM
}
