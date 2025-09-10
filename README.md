# bluepill-from-scratch

the following repository is a collection of simple bare-metal programs written for the STM32F103 completely from scratch, with absolutely no external code used and with no<sup>*</sup> prior knowledge assumed.

no libc, no cmsis, no vendor-provided code or ide.

each program should have a corresponding write-up _(of varying detail)_.

the idea was to demystify bare-metal embedded programming by hiding nothing and by verbalizing the entire process, while proving that almost all of the info we need comes straight from our hardware manuals or official GNU documentation.

everything will be written by us, including the linker script and startup code.

i had a lot of fun learning embedded programming this way, and I think I would've really liked a repository like this when I was starting out, so I thought I'd share it.

we'll start with a simple blinky program, and end with a PAW3395-based, 0-latency mouseclick, 1KHz USB HID gaming mouse on a custom PCB.

