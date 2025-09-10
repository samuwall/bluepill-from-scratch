/**********************************************************************************
 ** file            : st_usb.h
 ** description     : helper macros specific to the st usbfs device peripheral
 ** 
 **********************************************************************************/

#ifndef ST_USB_H
#define ST_USB_H

#include <stdint.h>
#include "device.h"

/* translate from usb2.0 dev desc bitfield to stm32 EPR bitfield */
const uint16_t translate_ep_type[] = {
    [USB_EP_ATTR_CONTROL]     = USB_EPR_EP_TYPE_CONTROL,
    [USB_EP_ATTR_ISOCHRONOUS] = USB_EPR_EP_TYPE_ISO,
    [USB_EP_ATTR_BULK]        = USB_EPR_EP_TYPE_BULK,
    [USB_EP_ATTR_INTERRUPT]   = USB_EPR_EP_TYPE_INTERRUPT,
};

/* 
 * these st usb registers need to be accessed and modified in such unsightly
 * ways that I really have no choice but to abstract them into these macros.
 * this doesn't really fit with prior design choices, but it's our best option.
 */

/* ----------------------------------------------------------------------------------- */
/* --- HELPER MACROS ----------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

/* --- epr stuff -- (see notes 1,2,3) --------------------------------- */

#define SET_TOGBITS(ep, msk, bits, extra_bits)                      \
    do {                                                            \
        uint16_t masked_reg = ((uint16_t) USB->EPR[ep]) & (msk);    \
        masked_reg ^= bits;                                         \
        USB->EPR[ep] = (uint16_t) ((masked_reg) | (extra_bits));    \
    } while (0)

#define USB_SET_EPR_EA(ep) \
    (USB->EPR[ep] = (uint16_t) ( ( ((uint16_t) USB->EPR[ep]) & (USB_EPR_NONTOGGLE_Msk) & (~USB_EPR_EA_Msk) ) | (ep | USB_EPR_RC_W0_Msk) ))

#define USB_SET_EPR_EP_TYPE(ep, type) \
    (USB->EPR[ep] = (uint16_t) ( ( ((uint16_t) USB->EPR[ep]) & (USB_EPR_NONTOGGLE_Msk) & (~USB_EPR_EP_TYPE_Msk) ) | (type | USB_EPR_RC_W0_Msk) ))

#define USB_SET_EPR_STAT_TX(ep, status) \
    SET_TOGBITS(ep, USB_EPR_NONTOGGLE_Msk | USB_EPR_STAT_TX_Msk, status, USB_EPR_RC_W0_Msk)

#define USB_SET_EPR_STAT_RX(ep, status) \
    SET_TOGBITS(ep, USB_EPR_NONTOGGLE_Msk | USB_EPR_STAT_RX_Msk, status, USB_EPR_RC_W0_Msk)

#define USB_CLR_EPR_DTOG_TX(ep) \
    (USB->EPR[ep] = (uint16_t) ( ( ((uint16_t) USB->EPR[ep]) & (USB_EPR_NONTOGGLE_Msk | USB_EPR_DTOG_TX_Msk) ) | (USB_EPR_RC_W0_Msk) ))

#define USB_CLR_EPR_DTOG_RX(ep) \
    (USB->EPR[ep] = (uint16_t) ( ( ((uint16_t) USB->EPR[ep]) & (USB_EPR_NONTOGGLE_Msk | USB_EPR_DTOG_RX_Msk) ) | (USB_EPR_RC_W0_Msk) ))

#define USB_CLR_EPR_CTR_RX(ep) \
    (USB->EPR[ep] = (uint16_t) ( ( ((uint16_t) USB->EPR[ep]) & (USB_EPR_NONTOGGLE_Msk & ~USB_EPR_CTR_RX_Msk) ) | (USB_EPR_CTR_TX_Msk) ))

#define USB_CLR_EPR_CTR_TX(ep) \
    (USB->EPR[ep] = (uint16_t) ( ( ((uint16_t) USB->EPR[ep]) & (USB_EPR_NONTOGGLE_Msk & ~USB_EPR_CTR_TX_Msk) ) | (USB_EPR_CTR_RX_Msk) ))

/* --- istr stuff -- (see note 4) ------------------------------------- */

#define USB_CLR_ISTR_RESET() \
    (USB->ISTR = (uint16_t) ~USB_ISTR_RESET_)

#define USB_CLR_ISTR_SUSP() \
    (USB->ISTR = (uint16_t) ~USB_ISTR_SUSP_)

#define USB_CLR_ISTR_WKUP() \
    (USB->ISTR = (uint16_t) ~USB_ISTR_WKUP_)

#define USB_CLR_ISTR_SOF() \
    (USB->ISTR = (uint16_t) ~USB_ISTR_SOF_)

/* --- pma stuff -- (see note 5) -------------------------------------- */

#define USB_PMA_BUF_START   0x40
#define USB_PMA_BASE        0x40006000

#define USB_PMA_EP_TX_ADDR(ep) \
    ((volatile uint32_t *) (USB_PMA_BASE + (((uint16_t) USB->BTABLE) + ep * 8 + 0) * 2) )

#define USB_PMA_EP_TX_COUNT(ep) \
    ((volatile uint32_t *) (USB_PMA_BASE + (((uint16_t) USB->BTABLE) + ep * 8 + 2) * 2) )

#define USB_PMA_EP_RX_ADDR(ep) \
    ((volatile uint32_t *) (USB_PMA_BASE + (((uint16_t) USB->BTABLE) + ep * 8 + 4) * 2) )

#define USB_PMA_EP_RX_COUNT(ep) \
    ((volatile uint32_t *) (USB_PMA_BASE + (((uint16_t) USB->BTABLE) + ep * 8 + 6) * 2) )

#define USB_SET_PMA_EP_TX_ADDR(ep, addr) \
    (*(USB_PMA_EP_TX_ADDR(ep)) = (uint16_t) (addr) )

#define USB_SET_PMA_EP_TX_COUNT(ep, count) \
    (*(USB_PMA_EP_TX_COUNT(ep)) = (uint16_t) (count) )

#define USB_SET_PMA_EP_RX_ADDR(ep, addr) \
    (*(USB_PMA_EP_RX_ADDR(ep)) = (uint16_t) (addr) )

#define USB_SET_PMA_EP_RX_COUNT(ep, count) \
    (*(USB_PMA_EP_RX_COUNT(ep)) = (uint16_t) (count) )

#define USB_GET_PMA_EP_TX_ADDR(ep) \
    ((uint16_t) *(USB_PMA_EP_TX_ADDR(ep)))

#define USB_GET_PMA_EP_TX_COUNT(ep) \
    ((uint16_t) *(USB_PMA_EP_TX_COUNT(ep)))

#define USB_GET_PMA_EP_RX_ADDR(ep) \
    ((uint16_t) *(USB_PMA_EP_RX_ADDR(ep)))

#define USB_GET_PMA_EP_RX_COUNT(ep) \
    ((uint16_t) *(USB_PMA_EP_RX_COUNT(ep)))

#define USB_GET_PMA_EP_TX_BUFF(ep) \
    (USB_PMA_BASE + (uint8_t *)(USB_GET_PMA_EP_TX_ADDR(ep) * 2))

#define USB_GET_PMA_EP_RX_BUFF(ep) \
    (USB_PMA_BASE + (uint8_t *)(USB_GET_PMA_EP_RX_ADDR(ep) * 2))


/*
 * note 1 : notice that when modifying non-toggle bits in the EPR register, we bitwise AND 
 *          the register with the mask of all non-toggle bits, resulting in all toggle 
 *          bits being set to 0. this is because the only way to preserve the current value 
 *          of 't' type bitfields is to write 0. this is counter-intuitive with the normal 
 *          reasoning regarding normal 'rw' type bitfields, where `REG = REG & ~MASK` is 
 *          sufficient to preserve non-mask bits since bitwise ANDing a 1 with a 1 preserves 
 *          the initial state. With toggle bits, writing that 1 back would clear it to 0.
 *
 * note 2 : this unusual behavior of toggle bits also affects how we clear and set these
 *          bits, as you can first see in `USB_CLR_EP_DTOG_TX` and `USB_SET_EP_STAT_TX`.
 * 
 *          you can see in `SET_TOGBITS` the logic for setting these toggle bits:
 * 
 *          first you get the current state of the toggle bits with the mask of 1s, then
 *          you XOR them with the desired state and write them back. so if the state is 
 *          [1, 1] and you want to write [1, 0], you write the XOR result [0, 1] so that 
 *          the first bit is unchanged and the second is toggled, becoming [1, 0].
 * 
 *          basically, the logic is, if the bits are different (XOR), write 1 to change it
 *          to what it needs to be.
 *
 * note 3 : STM32's USB hal_pcd driver also sets `CTR_TX` and `CTR_RX` when modifying
 *          the EPR register, for example in USB_SET_EPR_EA, they would do:
 *          reg = (reg & nontoggle_msk & ~epr_ea_msk) | (ep | CTR_TX | CTR_RX). these two
 *          bits are of type 'rc_w0', that is, 1 does nothing and 0 clears it. ig the idea 
 *          here is that between getting the current reg value and setting the bitfield, 
 *          CTR_RX for example could go from a 0 to a 1, and thus get prematurely cleared 
 *          by an innocuous macro like `USB_SET_EP_ADDR`. after all, rm0008 explicitly warns 
 *          against using read-modify-write without using 'invariant values'.
 * 
 *          i think the so-called 'invariant values' mentioned in rm0008 would be 0 for 't'
 *          bits and 1 for 'rc_w0' bits.
 *
 *          libopencm3 on the other hand does not do this.
 *
 *          TODO: ? probably.. makes sense to me.
 * 
 * note 4 : regarding the ISTR register, no reason to use read-modify-write at all, all
 *          writable registers are rc_w0 and written by hardware, so avoid non-atomicity
 *          altogether with just REG = ~MASK, which simply writes 1 to all bits you want 
 *          to leave alone.
 * 
 * note 5:  USB_PMA_BUF_START is the start addr offset of the endpoint-specific TX/RX packet 
 *          bufs. it is 0x40 (64) since the buffer descriptor table takes up the first 64 B.
 * 
 *          EPn_TX_ADDR + EPn_TX_COUNT + EPn_RX_ADDR + EPn_RX_COUNT = 8 bytes * 8 eps.
 * 
 *          we use the value to know where to start allocating new buffers as new endpoints 
 *          are set up.
 * 
 *          the USB_BTABLE register is just the start address of the actual btable (buffer
 *          descriptor table) found within the PMA, which describes the size (count) and addr
 *          of each endpoint's TX/RX packet buffer area. In 99% of cases, the value of 
 *          USB_BTABLE is just kept at 0. you use the EP_TX/RX_ADDR values stored in the
 *          pma btable to access the memory where you can read/write to the actual ep-specific
 *          TX/RX packet buffers, which are shared with the USB peripheral hardware. 
 *          (see rm0008 fig. 220)
 * 
 *          the PMA is called the USB SRAM in later versions of this peripheral, seems the
 *          big difference is that in our version, the buffer table descriptor register 
 *          addresses need to be multiplied by 2 to get the application-specific address.
 * 
 */
 
#endif