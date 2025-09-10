/**********************************************************************************
 ** file            : crt.c
 ** description     : no cmsis, no hal/libraries
 **                   stm32f103 i2c <--> ST24C16 eeprom 
 **
 **                   the goal of this program is to try and raise a CRT monitor's
 **                   VSCAN limit. in my experience, this limit is enforced via 
 **                   firmware, and the vertical refresh limit is often saved 
 **                   (along with other parameters) in an external EEPROM. in the 
 **                   case of my Viewsonic GS790, this EEPROM is the ST24C16.
 **
 **                   to do this, we will back up the original EEPROM contents,
 **                   search for the scan rate limits (e.g., 29 97 49 181), and 
 **                   raise the vert. max value to something like 255 -- allowing me
 **                   to turn a 180Hz monitor into a 254Hz monitor.
 **
 **                   the reason i think it could be possible is because it has been
 **                   done before with other monitors through servicing software, 
 **                   e.g., several sony models and the mitsubishi 2070SB. i have
 **                   personally done this with the sony CPD-G520. this would be a
 **                   first for directly modifying the EEPROM by desoldering it and
 **                   modifying the contents externally, as far as I know.
 **
 **                   this method is exciting because it means that any CRT monitor
 **                   which stores its monitor parameters in an EEPROM can have its
 **                   scan limits modified, whereas before this was only an option
 **                   for a select few monitors with leaked servicing software or 
 **                   EEPROM modification available in an OSD service menu.
 **
 **                   note: the ST24C16 is a 5V EEPROM. thankfully PB6 (SCL) and PB7
 **                   (SDA) are 5V tolerant pins, so we can just use pull-ups 
 **                   between SCL/SDA and 5V to create a 5V logic-level I2C bus.
 **
 **                   conclusion: there was a very promising series of bytes:
 **                   `50 180 30 95`. I changed the 180 to 250, soldered the eeprom
 **                   back in and it turned on but the 180Hz limit was still there.
 **                   monitors the eeprom hack works with: several sony monitors, 
 **                   diamond pro 2070sb. monitors it doesnt work with: viewsonic 
 **                   GS790 :(. i still believe it's possible for other monitors!
 **
 **********************************************************************************/

#include <stdint.h>
#include "device.h"
#include "rcc.h"
#include "gpio.h"
#include "i2c7.h"
#include "SEGGER_RTT.h"

#define NUM_BLOCKS 8
#define BLOCK_SIZE 256

static void delay_ms(uint32_t ms) {
    uint32_t n = (ms * 72000) / 5;
    while (n--) {
        __asm__("nop");
    }
}

static void clock_setup(void) {

    set_sysclk_72mhz();
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN_;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN_;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN_;

}

static void gpio_setup(void) {

    /* PB6 -> AF open-drain 50MHz */
    GPIOB->CRL = (GPIOB->CRL & ~(GPIO_CRL_CNFMODE6_Msk)) | (GPIO_CNFMODE_OUTPUT_AF_OPENDRAIN_50MHZ << GPIO_CRL_CNFMODE6_Shft);

    /* PB7 -> AF open-drain 50MHz */
    GPIOB->CRL = (GPIOB->CRL & ~(GPIO_CRL_CNFMODE7_Msk)) | (GPIO_CNFMODE_OUTPUT_AF_OPENDRAIN_50MHZ << GPIO_CRL_CNFMODE7_Shft);

}

static void i2c_setup(void) {

    /* i2c peripheral must be disabled before configuring */
    I2C1->CR1 &= ~I2C_CR1_PE_;

    /* freq bits must be configured with APB clock frequency value, in this case APB1 is 36mhz */
    I2C1->CR2 = (I2C1->CR2 & ~(I2C_CR2_FREQ_Msk)) | (36 << I2C_CR2_FREQ_Shft);

    /* sm mode (100KHz) for old ass ST24C16 */
    I2C1->CCR &= ~I2C_CCR_FS_;

    /* for f_scl = 100khz and f_pclk1 = 36mhz and R_p = 4.7KOhm, the stm32f103x8 datasheet suggests a CCR of 0xB4 */
    I2C1->CCR = (I2C1->CCR & ~(I2C_CCR_CCR_Msk)) | (0xB4 << I2C_CCR_CCR_Shft);

    /* trise = "maximum duration of scl feedback loop in master mode" 
     * the scl feedback loop is comprised of: the scl rising time, noise filter delay, 
     * and synchronization delay with APB1. until these delays are elapsed, SCL is 
     * still rising and cannot be confirmed high. 
     * 
     * 1. master releases SCL, immediately starts high counter, it starts to go high.
     * 2. slave might be stretching the clock (pulling SCL low), so we need to confirm
     *    that SCL has actually gone high. otherwise we'll be pulling and releasing 
     *    SCL as if we're actually doing something, losing track of time. 
     * 3. if high, CONTINUE counting, if low, pause counter, and the peripheral will 
     *    enter a wait state continuously monitoring SCL to go high.
     * 
     * TRISE is a one-time counter that determines how long we should wait after
     * releasing SCL before checking the state of SCL. the goal behind this counter
     * is to create a more stable SCL frequency, as the SCL feedback loop duration
     * may vary over time, meaning the exact high period duration may vary, but at
     * least it will always hit the minimum guaranteed SCL high period.
     * 
     * it seems that a lower trise value, even 0, cannot hurt i2c functionality 
     * (https://community.st.com/t5/stm32-mcus-products/i2c-trise/td-p/511255). 
     * on the other hand, too high of a trise threatens to break i2c communication, as 
     * the high counter will run for too long (e.g., while scl is low (clock 
     * stretching)) and the high period will end prematurely, leaving high/low 
     * unbalanced or otherwise falling out of spec (< min. gtd. high period).
     * 
     * at least, this is the impression i got from rm0008 p.758. newer stm32 mcus do
     * away with this counter altogether. (i can see why)
     * 
     * trise = (t_r(scl) / t_pclk1) + 1
     * according to datasheet, for an f_scl of 100khz, t_r(scl) = 1000ns
     * t_pclk1 = periph clock period = 1 / 36mhz = 27.777ns = 28ns
     * trise = (1000ns / 28ns) + 1 = (35.7) + 1 = 36.7 = 36
     */
    I2C1->TRISE = (I2C1->TRISE & ~(I2C_TRISE_TRISE_Msk)) | (36 << I2C_TRISE_TRISE_Shft);

    /* re-enable i2c peripheral */
    I2C1->CR1 |= I2C_CR1_PE_;

}

/* print entire eeprom contents and save to 2d array param
 */
static void dump_eeprom(uint8_t data[NUM_BLOCKS][BLOCK_SIZE]) {

    uint8_t eep_addr;
    uint8_t byte_addr;

    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        SEGGER_RTT_printf(0, "%3d ", i);
    }

    for (uint8_t block = 0; block < NUM_BLOCKS; block++) {

        eep_addr = 0b1010000 | block;
        byte_addr = 0b00000000;

        i2c7_write(I2C1, eep_addr, &byte_addr, sizeof(byte_addr), I2C_NOSTOP);
        i2c7_read(I2C1, eep_addr, data[block], BLOCK_SIZE);

        SEGGER_RTT_printf(0, "\n\nblock %d:\n", block);
        for (size_t i = 0; i < BLOCK_SIZE; i++) {
            SEGGER_RTT_printf(0, "%3d ", data[block][i]);
        }
    }

    SEGGER_RTT_WriteString(0, "\n");
}

int main(void) {

    clock_setup();
    gpio_setup();
    i2c_setup();

    uint8_t read[NUM_BLOCKS][BLOCK_SIZE] = {0};
    uint8_t write[2] = {0};
    uint8_t eeprom_slave_addr;
    uint8_t byte_address;

    SEGGER_RTT_WriteString(0, "\nInitial EEPROM data: \n\n");
    dump_eeprom(read);

    /* block 6 byte 96 */
    eeprom_slave_addr = 0b1010110;
    byte_address = 96;

    /* write the byte address first, then write value */
    write[0] = byte_address;
    write[1] = 250;

    /* byte write */
    i2c7_write(I2C1, eeprom_slave_addr, write, sizeof(write), I2C_STOP);

    /* wait for eeprom internal write cycle -- max 10ms */
    delay_ms(10);

    SEGGER_RTT_WriteString(0, "\nModified EEPROM data: \n");
    dump_eeprom(read);

    for (;;) { 
        /* done */ 
    }

}
