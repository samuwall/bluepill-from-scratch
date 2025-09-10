/************************************************
** file           : minlink.ld 
** description    : minimal linker script
**
**
************************************************/

SECTIONS
{
    . = 0x8000000;
    .text :
    {
        *(.text)
    }
}

