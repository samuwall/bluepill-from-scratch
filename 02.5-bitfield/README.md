# `02.5-bitfield`

### tl;dr

full source [here]().

this program explores a less common style of peripheral access for bare-metal that I think is worth looking at, even if I decided against it for this repository's purposes.

it uses C bit-fields to represent each bitfield of a peripheral register, which can then be accessed like struct members:

```c
GPIOC->CRH.bit.CNF_MODE13 = 0b0110;
```

vs.

```c
GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNF_MODE13_Msk)) | (0b0110 << GPIO_CRH_CNF_MODE13_Shft);
```

### from scratch

This approach looks really clean, is self-documenting, and results in the usage of the `bfi` (bit field insert) instruction, saving 4 bytes compared to the normal RMW bit-masking method, which needs to call `bic` and then `orr` to achieve the same thing.

It seems this (quite useful) instruction otherwise goes completely unused.

If you do any googling of this method though, the consensus seems to be overwhelmingly negative. Their problem with it is that it is 'implementation-defined', meaning it isn't strictly defined by the ISO C standard what the order, padding, etc. of struct bitfields should be. Compilers can decide for themselves how they want to handle it, meaning changing compilers or compiler versions could technically break your code. This is a scary thought for many, but I wouldn't completely write it off. 

It seems compilers have come to a gentleman's agreement to implement them in the way you'd expect. I don't think ARM themselves would rely on them in their CMSIS-Core headers for their CPUs (which like 99% of embedded code relies on) and expose this option in their official packaging utilities (`svdconv`) [[x]](https://open-cmsis-pack.github.io/svd-spec/main/svd_SVDConv_pg.html) if this wasn't the case. If you look around though, most embedded code simply uses RMW with bit-masking instead of these struct bit fields. RMW with bit-masking is definitely not implementation-defined. Don't fix it if it ain't broke is a popular mindset in embedded. It's why you'll see people using C89 and compiling with GCC 3.0 in _current year_. 

I decided to go with bit-masking to follow more standard practice for the sake of this repository.

If you wanted to go the bit-field route, I would recommend using `SVDConv device.svd --generate=header --fields=struct`, which takes the device SVD file as input and outputs a header file with all of the bitfield structs for every peripheral. 

To be fair, using SVDConv or the ST-provided CMSIS header is smart even when going the bit-mask route. We stubbornly define each peripheral manually as we go along just so that we can honestly say every single line of code was written by us. In real life though, that's pretty unneccessary and error-prone.

> Other vendors like Nordic dodge this whole read-modify-write issue by not having reserved bits at all, instead reserving entire registers, saving many needless `ldr/bic/orr` operations. They also provide plenty of atomic `SET` / `CLR` registers, meaning read-modify-write is almost never necessary. I like Nordic.

Here's the same blinky program from `02-struct`, using struct bit-fields instead of RMW bitmasking.

```c
#include <stdint.h>
#include "device.h"

static void delay(uint32_t n) {
    while (n--) {
        __asm__("nop");
    }
}

int main(void) {

    RCC->APB2ENR.bit.IOPCEN = 1;

    GPIOC->CRH.bit.CNF_MODE13 = GPIO_CNFMODE_OUTPUT_GP_OPENDRAIN_2MHZ;

    for(;;) {

        GPIOC->ODR.bit.ODR13 = 1;
        delay(500000);
        GPIOC->ODR.bit.ODR13 = 0;
        delay(500000);

    }

}
```

We saved x bytes compared to `02-struct`.