/********************************************************************
 ** file         : stm32f103c8t6.ld
 ** description  : libc linker script
 **
 **
 ********************************************************************/

MEMORY
{
    RAM   (rwx) : ORIGIN = 0x20000000,  LENGTH = 20K
    FLASH (rx)  : ORIGIN = 0x8000000,   LENGTH = 64K
}

_estack = ORIGIN(RAM) + LENGTH(RAM); 
_Min_Stack_Size = 0x400;

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
        __bss_start__ = _sbss;
        *(.bss*)
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM

    . = ALIGN(8);
    _end = . ;

}

