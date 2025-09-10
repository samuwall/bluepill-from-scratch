
# `00-setup`

### tl;dr

full source [here](https://github.com/samuwall).

this repository uses the STM32F103C8T6 "Blue Pill", but you can use any STM32F103 board.

the following programs are all IDE-agnostic. all you need is `arm-none-eabi-gcc` and a way to flash/debug the target (j-link, openocd, st-link, etc.)

personally, i used vscode with the `Cortex-Debug` extension and a J-Link EDU Mini. everything works out of the box with the provided `.vscode` folder should you wish to go that route.

this is the first of the write-ups, introducing the hardware, and detailing how to compile/flash our first program.

### from scratch:

### hardware

We will be using the ubiquitous "Blue Pill" dev board, which is essentially a breakout board for the `STM32F103C8T6` microcontroller. That is, it simply "breaks out" all of the tiny pins of the microcontroller into larger standard-pitch pin headers for easy connections. A microcontroller (MCU) is an entire mini-computer contained in a single chip, in that it not only contains a CPU for processing, but also numerous "peripherals" for communicating with the outside world, including UART, I2C, SPI, GPIOs, USB, etc. It also contains volatile memory (RAM) for storing dynamic values like the global variables, stack, and heap of a program, and usually non-volatile memory (FLASH) for storing the actual program code and constant data. 

Microcontrollers like the `STM32F103C8T6` are generally designed to be low-cost, low-power, and space-efficient. Having a tiny/slow CPU and as little RAM and FLASH as possible greatly benefits all three of those categories. And so, the `STM32F103C8T6` only has a 72MHz CPU, 20KB of RAM and 64KB of FLASH, far too little to run even the most lightweight Linux-based OS you can think of. This means we have two options: use a Real-Time Operating System (RTOS) which provides us with OS features like task scheduling, synchronization primitives, etc. while being fast enough to respond to events in "real-time", or, go the "bare-metal" route, and use loops and interrupts to accomplish whatever we need. 

>#### "bare-metal"?
>
>Bare-metal is an ambiguous term, the most official/common definition is probably just "No-OS", though it might also refer to particularly "direct" hardware access. The following programs are bare-metal by both definitions of the term, so I'll just use it ambiguously.
>
>#### why bare-metal?
>
>I figured that starting out the bare-metal route would later allow me to better appreciate the use of RTOS's and/or hardware abstraction layers. Plus, my end-goal project as I was starting out was a simple USB mouse. Does a mouse really need its own operating system?
>
>We could have saved time using pre-existing libraries, HALs, or RTOS's. In our approach though, nothing is left to the imagination, everything is done from scratch, and I found that really fun and interesting.

Unfortunately it isn't enough to just plug the bluepill into your PC through USB and start flashing and debugging your own programs -- that would require an "on-board debugger", which the bluepill lacks. So, you'll also need a debug probe, which is just another microcontroller that bridges between the target (the bluepill) via the debug pins (SWD/JTAG), and the host (your PC) via USB. I used the J-Link EDU Mini, which can be found on mouser/digikey for $60, and sometimes cheaper on eBay. 

You can of course use any debug probe and software combination you want, but I can only go into the setup details of what I personally used.

If you use the J-Link EDU Mini, you'll also need a breakout board for the 2x5 1.27mm IDC cable in order to connect it to the bluepill's 2.54mm SWD pins.

The power also needs to be provided externally, either through USB or a 5v/3.3v pin on the pin header.

### software

Now we need a way to compile our code into a binary suitable for the `ARM Cortex-M3` CPU found in our `F103`. This means we need a cross compiler. We'll use the tried and true `arm-none-eabi-gcc`. We have `arm`, because our target uses an ARM core, `none`, because our target has no OS, and `eabi` because it complies with the `Embedded Application Binary Interface`, which is just some set of standards for object files targeting embedded systems.

```txt
pacman -S arm-none-eabi-gcc
```

We'll also need the host-side software for the J-Link debug probe,

```txt
yay -S jlink-software-and-documentation
```

and with just two packages, we can now simply compile,

```txt
arm-none-eabi-gcc -mcpu=cortex-m3 -nostdlib -ffreestanding -g3 -Og -T link.ld blink.c -o blink.elf
```

and flash:

```txt
$ JLinkExe -device STM32F103C8 -if SWD -speed 4000
J-Link>connect
J-Link>reset
J-Link>loadfile blink.elf
J-Link>reset
J-Link>exit
```

[ add bluepill blinking GIF ]

If we want to debug, we simply install `gdb` for arm-none-eabi targets,

```txt
pacman -S arm-none-eabi-gdb
```

start JLinkGDBServer,

```txt
$ JLinkGDBServer -device STM32F103C8 -if SWD -speed 4000 -port 2331
Connected to target
Waiting for GDB connection...
```

and in a separate terminal, connect to the GDB server which was opened on port 2331 and start debugging.

```txt
$ arm-none-eabi-gdb blink.elf

(gdb) target remote localhost:2331
Remote debugging using localhost:2331
0x08000032 in delay (n=1000) at blink.c:10
10              for (uint32_t j = 0; j < n; j++) {
(gdb) mon reset
Resetting target
(gdb) load
Loading section .isr_vector, size 0x10 lma 0x8000000
Loading section .text, size 0x114 lma 0x8000010
Start address 0x080000b0, load size 292
Transfer rate: 142 KB/sec, 146 bytes/write.
(gdb) mon reset
Resetting target
(gdb) b main
Breakpoint 1 at 0x8000054: file blink.c, line 18.
(gdb) continue
Continuing.

Breakpoint 1, main () at blink.c:18
18          RCC_APB2ENR = RCC_APB2ENR | (1<<4);
```

>In the above example, `mon reset` also halts the CPU, allowing us to stop at `main`.

We send special J-Link-specific commands through to the GDB server by prepending them with `monitor`/`mon`, e.g., `mon reset` to reset the target. The full list of `mon` commands is available on [this page](https://kb.segger.com/J-Link_GDB_Server).

That's all there is to it, we've flashed and debugged an MCU with no IDE.

I guess doing all that manually every time is a bit of work, and debugging through the GDB CLI is a little archaic, which is why we'll use Makefiles and attach the J-Link GDB server to a graphical debugger.

If you're interested in the Visual Studio Code + Cortex-Debug route, see the following tab:

> <details> <summary> vscode </summary>
> 
> Simply install vscode and the [C/C++]() and [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) extensions.
> 
> There are three related extensions which Cortex-Debug relies on as front-ends. VScode hopefully prompts you to install these. If not, search for and install [Peripheral Viewer](https://marketplace.visualstudio.com/items?itemName=mcu-debug.peripheral-viewer), [MemoryView](https://marketplace.visualstudio.com/items?itemName=mcu-debug.memory-view), and [debug-tracker-vscode](https://marketplace.visualstudio.com/items?itemName=mcu-debug.debug-tracker-vscode).
> 
> Clone this `bluepill-from-scratch` repository and open it with vscode, or just copy the `.vscode` folder over to your own folder and do your own thing.
>
> Finally, you can edit the contents of the `.vscode` folder to reflect your own setup. For example, you might modify `launch.json` by setting `servertype` to `openocd`, or choose a different target with `device`. There's a CMSIS SVD file for the STM32F103 included in the repo, which you might replace with any other device's SVD. The SVD provides bitfield definitions for every peripheral register, which lets you see their contents in real-time as you debug.
> 
> You should also edit `.vscode/c_cpp_properties.json` to reflect whichever program you are currently working on as you go, so that intellisense can keep up.
> 
> ```json
> "includePath": [
>     "${workspaceFolder}/01-blink/**"
> ],
> ```
>
> That's it!
>
> You can now just press `Run and Debug` on a program's main source file and vscode will build, flash, and debug the program.
>
> </details>

<br>

Don't worry about any of the code or commands used, the next programs will explain everything in detail.
