# `02-struct`

### tl;dr

full source [here]().

in the previous program we worked on a minimal blinky example.

in this program we establish the coding style that the rest of the repository uses, using structs and bitfield macros for register access. 

we'll also write the startup code, linker script, and Makefile that we'll be using moving forward -- even up to the final USB mouse program.

the cool part is that there's only marginally more code involved compared to the minimal example.

we also briefly showcase debugging with `Cortex-Debug`, stepping through the code and using the SVD viewer to see the state of every hardware register at any point while halted.

### from scratch

How can we improve upon `01-blink`, move beyond its strict minimalism, and get a nice foundation for the subsequent programs?

We can just think about the aspects of `01-blink` that probably wouldn't scale well for much larger programs.

If we keep the peripheral access as it is, the top of each source file would get filled with peripheral register macros as we incorporate more of them. We'd also have hardcoded bitfield values in our program code, hurting readability. So, we'll implement cleaner peripheral access and abstract it into a header file.

If we keep the vector table and exception handlers as they are, we'd take up unneccessary space from every `main()` file. We also wouldn't be able to use global or static storage duration variables, as `01-blink` does not copy `.data` values from FLASH to RAM, nor zero-out the allocated memory for `.bss`. So, we'll abstract the vector table and RAM initiliaztion into a separate `.c` file that runs prior to `main()`, while making slight changes to the linker script to support the `.data`/`.bss` initialization.

If we organize programs into multiple `.c` files, generate various outputs with our build, and switch between debug/release compilation flags often, etc., we'd waste a lot of time manually copy and pasting verbose commands into the terminal. So, we'll implement a build system to streamline these things, consisting of a simple Makefile.

With just these three upgrades, we'll have a solid foundation for the rest of the programs.

#### peripheral access

Let's start with optimizing peripheral access.

In `01-blink`, we accessed peripheral registers like so:

```c
#define RCC_APB2ENR *((volatile uint32_t *) 0x40021018)

RCC_APB2ENR = RCC_APB2ENR | (1 << 4);
```

We can imagine that we probably don't want to manually define each register with its own complicated macro at the top of every source file that needs access.

We can also see that `(1 << 4)` isn't very self-documenting.

Instead, we'll create a header file that contains all of our peripheral access, which can then be included wherever necessary. We'll call it `device.h`.

```c
/********************************************************************
** file         : device.h
** description  : structs and macros for core/device peripheral access
**
** device       : STM32F103
**
********************************************************************/

#ifndef DEVICE_H
#define DEVICE_H

#define RCC_APB2ENR *((volatile uint32_t *) 0x40021018)

#endif DEVICE_H
```

We should find a neater way to define each register. Luckily, C has structs, wherein we can define a larger data structure (the entire peripheral in this case) as being organized into / comprised of smaller members (the individual registers in this case), with the offsets of each register/member being automatically calculated from the member widths. These members can then be accessed in a readable way e.g., `RCC->APB2ENR`.

```c
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

#define RCC ((RCC_T *)  0x40021000)

#endif
```

...


Globals which are declared as `const` are placed by the compiler in `.rodata` rather than `.text`, but you don't have to just take my word for it. We can verify this with `arm-none-eabi-objdump -h struct.o`, which shows all of the sections the compiler has generated, including `.rodata` with our x bytes of data. Thus, we'll have to add `.rodata` to our linker script. 

There are also two mysterious sections, `.comments` and `.ARM.attributes`. We can display their hex contents decoded into ASCII to hopefully figure out what they contain, with `arm-none-eabi-objdump -h -s struct.o`. 

The `.comments` section just seems to contain the compiler name and version that generated the object file, and `.ARM.attributes` has some strings like `armv7` and `eabi`, suggesting it contains information about our target. These sections are non-allocatable, non-loadable, and as such don't really concern us. Just some metadata it seems. Since our linker script doesn't explicitly mention these input sections, `ld` "intelligently" places them for us based on some set of rules. These are called "orphan sections" [[x]]().


(linker) now to specify the addresses of these output sections, we'll use a syntax that's a bit more self-descriptive than the hardcoded location pointer assignment `. = 0x800000`. Using the `MEMORY` command, we can instead refer to these addresses with an identifier, and specify the starting VMA of output sections with a simple `> FLASH` for example. We can specify both the VMA _and_ LMA using the syntax `> RAM AT >FLASH`.