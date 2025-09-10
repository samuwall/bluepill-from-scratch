/**********************************************************************************
 ** file            : usb.c
 ** description     : stm32f103 usbfs device driver (usb hw periph. interface)
 ** 
 **
 **********************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "device.h"
#include "gpio.h"
#include "utils.h"
#include "usb.h"
#include "usb_ep0.h"
#include "st_usb.h"

#include "SEGGER_RTT.h"

/* avoid dynamic mem allocation by using global instead */
usb_device usbfs_dev;

/* st-specific driver helpers */
uint16_t pma_top;

/* ----------------------------------------------------------------------------------- */
/* --- USB DRIVERS ------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------- */

void usb_enable_isr(void) {
    NVIC->ISER[NVIC_USB_LP_CAN_RX0_IRQ / 32] = (1 << (NVIC_USB_LP_CAN_RX0_IRQ % 32));
}

usb_device * usb_init(const struct usb_device_descriptor *dev_desc, 
                      const struct usb_configuration_descriptor *config, 
                      const struct usb_string_descriptor * const *str_descs, 
                      int num_string_descs) {

    /* configure device */
    usb_device *usb_dev     = &usbfs_dev;
    usb_dev->dev_desc       = dev_desc;
    usb_dev->config         = config;
    usb_dev->str_descs      = str_descs;
    usb_dev->num_str_descs  = num_string_descs;
    usb_dev->status         = (config->bmAttributes & USB_CFG_ATTR_SELF_POWERED) ? 1 : 0;

    usb_dev->user_ctr_callback[0][USB_TRANSACTION_SETUP] = _usb_ep0_setup;
    usb_dev->user_ctr_callback[0][USB_TRANSACTION_OUT]   = _usb_ep0_out;
    usb_dev->user_ctr_callback[0][USB_TRANSACTION_IN]    = _usb_ep0_in;

    usb_dev->user_set_config_callback = NULL;

    for (int i = 0; i < MAX_USER_EP0_REQ_HANDLER; i++) {
        usb_dev->user_ep0_req_handler[i].cb = NULL;
    }

    return usb_dev;
}

void usb_start(usb_device *dev) {

    (void)dev;

    /* make sure D+ (PA12) is not controlled by USB peripheral */
    RCC->APB1ENR &= ~RCC_APB1ENR_USBEN_;

    /* re-enumerate: see note 1 */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN_;
    GPIOA->CRH = (GPIOA->CRH & ~(GPIO_CRH_CNFMODE12_Msk)) | (GPIO_CNFMODE_OUTPUT_GP_OPENDRAIN_2MHZ << GPIO_CRH_CNFMODE12_Shft);
    gpio_clear(GPIOA, GPIO12);
    for (uint32_t i = 0; i < 800000; i++) {
        __asm__("nop");
    }
    gpio_set(GPIOA, GPIO12);

    /* enable usb clock, take over D+ and D- pins (rm0008 p.168 t.29) */
    RCC->APB1ENR |= RCC_APB1ENR_USBEN_;

    /* initial values */
    USB->CNTR   = (uint16_t) 0;
    USB->BTABLE = (uint16_t) 0;
    USB->ISTR   = (uint16_t) 0;

    /* enable interrupts */
    USB->CNTR = (uint16_t) (USB_CNTR_RESETM_ | USB_CNTR_CTRM_ | USB_CNTR_SUSPM_ | USB_CNTR_WKUPM_);

}

void usb_stop(usb_device *dev) {

    (void)dev;

    /* disable interrupts */
    USB->CNTR = (uint16_t) 0;

    /* clear interrupt status reg */
    USB->ISTR = (uint16_t) 0;

    /* reset state machine */
    USB->CNTR = (uint16_t) USB_CNTR_FRES_;
    USB->CNTR = (uint16_t) 0;

    /* disable peripheral */
    USB->CNTR = (uint16_t) (USB_CNTR_PDWN_ | USB_CNTR_FRES_);

    /* disable USB clock */
    RCC->APB1ENR &= ~RCC_APB1ENR_USBEN_;

}

static uint16_t usb_set_ep_rx_bufsize(usb_device *dev, uint8_t ep, uint32_t size) {

    /* calculate BL_SIZE and NUM_BLOCK based on `size`, then write to PMA.
     * return actual size of allocated buffer (potentially got rounded up).
     *
     * for necessary context, see rm0008 p.650 
     *
     * "Due to the restrictions on the number of available bits, buffer size is 
     * represented using the number of allocated memory blocks"
     * "If BL_SIZE=0, the memory block is 2 byte large"
     * "If BL_SIZE=1, the memory block is 32 bytes large"
     */

    uint16_t buf_size;
    (void)dev;

    if (size > 62) {
        /* div by 32, AND it with a 5-bit mask to keep within NUM_BLOCK field */
        /* sub by 1 to ensure that sizes like 64 map correctly to 1 NUM_BLOCK */
        size = ((size - 1) >> 5) & 0b11111;
        buf_size = (size + 1) << 5;
        /* set bit 15, BL_SIZE */
        size |= (1 << 5);
    }
    else {
        /* add 1 to round up odd numbers,
         * then divide the size by 2, since each block will be multiplied by 
         * block size (2 bytes) to represent the actual buffer size */
        size = (size + 1) >> 1;
        /* left shift 1 reverses the div by 2 to reflect the actual size of the buffer */
        buf_size = size << 1;
    }

    /* `size << 10` correctly positions size over bits 15:10, BL_SIZE and NUM_BLOCK */
    USB_SET_PMA_EP_RX_COUNT(ep, size << 10);
    return buf_size;
}

void usb_setup_ep(usb_device *dev, uint8_t addr, uint16_t type, uint16_t max_size, 
                    usb_endpoint_callback ctr_callback) {

    uint8_t  dir = (addr >> 7) & 0b1;  /* extract dir from msb (8th bit) */
    uint8_t  ep  = addr & 0b01111111;  /* extract ep number from first 7 bits */
    uint16_t buf_size;

    USB_SET_EPR_EA(ep);
    USB_SET_EPR_EP_TYPE(ep, translate_ep_type[type]);

    if (ep == 0) {
        /* cfgr TX */
        USB_SET_PMA_EP_TX_ADDR(ep, pma_top);
        pma_top += max_size;
        USB_CLR_EPR_DTOG_TX(ep);
        USB_SET_EPR_STAT_TX(ep, USB_EPR_STAT_TX_NAK);

        /* cfgr RX */
        USB_SET_PMA_EP_RX_ADDR(ep, pma_top);
        buf_size = usb_set_ep_rx_bufsize(dev, ep, max_size);
        pma_top += buf_size;
        USB_CLR_EPR_DTOG_RX(ep);
        USB_SET_EPR_STAT_RX(ep, USB_EPR_STAT_RX_VALID);
        
        return;
    }

    /* IN ep */
    if (dir == 1) {
        USB_SET_PMA_EP_TX_ADDR(ep, pma_top);
        if (ctr_callback) {
            dev->user_ctr_callback[ep][USB_TRANSACTION_IN] = ctr_callback;
        }
        pma_top += max_size;
        USB_CLR_EPR_DTOG_TX(ep);
        USB_SET_EPR_STAT_TX(ep, USB_EPR_STAT_TX_NAK);
    }

    /* OUT ep */
    if (dir == 0) {
        USB_SET_PMA_EP_RX_ADDR(ep, pma_top);
        buf_size = usb_set_ep_rx_bufsize(dev, ep, max_size);
        if (ctr_callback) {
            dev->user_ctr_callback[ep][USB_TRANSACTION_OUT] = ctr_callback;
        }
        pma_top += buf_size;
        USB_CLR_EPR_DTOG_RX(ep);
        USB_SET_EPR_STAT_RX(ep, USB_EPR_STAT_RX_VALID);
    }

}

void usb_set_device_address(usb_device *dev, uint8_t addr) {

    (void)dev;
    USB->DADDR = (addr & USB_DADDR_ADDR_Msk) | (USB_DADDR_EF_);

}

void usb_ep_set_stall(usb_device *dev, uint8_t addr) {

    (void)dev;

    #if DBG >= 1
    SEGGER_RTT_printf(0, "    SET_STALL\n");
    #endif

    uint8_t dir = (addr >> 7) & 0b1;  /* extract dir from msb (8th bit) */
    uint8_t ep  = addr & 0b01111111;  /* extract ep number from first 7 bits */

    if ((dir == 1) || (ep == 0)) {
        USB_SET_EPR_STAT_TX(ep, USB_EPR_STAT_TX_STALL);
    }
    else {
        USB_SET_EPR_STAT_RX(ep, USB_EPR_STAT_RX_STALL);
    }
    
}

void usb_ep_set_clr_nak(usb_device *dev, uint8_t addr, uint8_t nak) {

    (void)dev;

    /* no point in setting NAK for an IN endpoint, default state is NAK, only we can write VALID */
    if (addr & 0b10000000) {
        return;
    }

    /* set or clear NAK */
    USB_SET_EPR_STAT_RX(addr, nak ? USB_EPR_STAT_RX_NAK : USB_EPR_STAT_RX_VALID);

}

static void usb_write_to_pma(volatile void *pma_dest, const void *buf, uint16_t len) {

    const uint16_t *lbuf = buf;    /* local copy of buf where data is broken up into 16-bit chunks */
    volatile uint32_t *pma = pma_dest;   

    /* loop half as many times as there are bytes, since we are copying in 16-bit chunks */
    /* ensure the last byte of an odd number of bytes is not lost by rounding up */
    for (len = (len + 1) >> 1; len; len--) {
        /* with each postfix increment, we point to the next 32-bit/16-bit block respectively */
        *pma++ = *lbuf++;
    }

}

uint16_t usb_ep_write_packet(usb_device *dev, uint8_t addr, const void *buf, uint16_t len) {

    (void)dev;
    uint8_t ep = addr & 0b01111111;

    /* if tx buffer is valid, it already contains data yet to be requested and sent */
    if ((USB->EPR[ep] & USB_EPR_STAT_TX_Msk) == USB_EPR_STAT_TX_VALID) {
        /* error code to distinguish from ZLP success */
        return 0xffff;
    }
    
    /* rm0008 23.4.2: data transmission */
    usb_write_to_pma(USB_GET_PMA_EP_TX_BUFF(ep), buf, len);
    USB_SET_PMA_EP_TX_COUNT(ep, len);
    USB_SET_EPR_STAT_TX(ep, USB_EPR_STAT_TX_VALID);

    return len;
}

static void usb_read_from_pma(void *buf, const volatile void *pma_src, uint16_t len) {

    uint16_t *lbuf = buf;
    const volatile uint32_t *pma = pma_src;
    uint8_t odd = len & 1;

    for (len = len >> 1; len; len--) {
        *lbuf++ = *pma++;
    }
    /* dont round up, just get last byte */
    if (odd) {
        *(uint8_t *)lbuf = *(uint8_t *)pma;
    }

}

uint16_t usb_ep_read_packet(usb_device *dev, uint8_t addr, void *buf, uint16_t len) {

    (void)dev;
    uint8_t ep = addr & 0b01111111; /* redundant? dir always 0 (OUT)?*/

    if ((USB->EPR[ep] & USB_EPR_STAT_RX_Msk) == USB_EPR_STAT_RX_VALID) {
        /* error code to distinguish from ZLP success */
        return 0xffff;
    }

    /* rm0008 23.4.2: data reception */
    
    /* can't read more bytes than have been received, so use the smaller of the two */
    len = MIN(USB_GET_PMA_EP_RX_COUNT(ep) & 0b01111111111, len); /* mask away bl_size and num_block, only get count [9:0] */
    usb_read_from_pma(buf, USB_GET_PMA_EP_RX_BUFF(ep), len);
    USB_CLR_EPR_CTR_RX(ep);
    USB_SET_EPR_STAT_RX(ep, USB_EPR_STAT_RX_VALID);

    return len;
}

void usb_setup_acked(usb_device *dev) {

    usb_ep_set_clr_nak(dev, 0, 1);
    dev->ep0.req_cmpl = NULL;

}

void usb_prepare_for_status(usb_device *dev, uint8_t dir) {

    if (dir == USB_STATUS_IN) {
        /* write ZLP */
        usb_ep_write_packet(dev, 0, NULL, 0);
    }
    else if (dir == USB_STATUS_OUT) {
        /* clear NAK to enable reception of ZLP */
        usb_ep_set_clr_nak(dev, 0, 0);
    }

}

void usb_status_acked(usb_device *dev, uint8_t dir) {

    struct usb_setup_data *req = &(dev->ep0.req);

    if (dir == USB_STATUS_IN) {
        /* set address if completed transfer was a SET_ADDRESS request */
        if ((req->bmRequestType == 0) && (req->bRequest == USB_REQ_SET_ADDRESS)) {
            usb_set_device_address(dev, req->wValue);
        }
    }
    else if (dir == USB_STATUS_OUT) {
        /* read ZLP */
        usb_ep_read_packet(dev, 0, NULL, 0);
    }

    /* request complete, run callback if set */
    if (dev->ep0.req_cmpl) {
        dev->ep0.req_cmpl(dev, req);
    }

}

void usb_reset_endpoints(usb_device *dev) {

    /* reset all endpoints (except control) */
    for (uint8_t ep = 1; ep < 8; ep++) {
        USB_SET_EPR_STAT_TX(ep, USB_EPR_STAT_TX_DISABLED);
        USB_SET_EPR_STAT_RX(ep, USB_EPR_STAT_RX_DISABLED);
    }

    pma_top = USB_PMA_BUF_START + (2 * dev->dev_desc->bMaxPacketSize0);

}

static void usb_reset(usb_device *dev) {

    #if DBG >= 1
    SEGGER_RTT_printf(0, "RESET\n");
    #endif

    pma_top = USB_PMA_BUF_START;
    dev->configured = 0;

    usb_setup_ep(dev, 0, USB_EP_ATTR_CONTROL, dev->dev_desc->bMaxPacketSize0, NULL);
    usb_set_device_address(dev, 0);

}

static void usb_ctr(usb_device *dev, uint16_t istr) {

    uint8_t ep = istr & USB_ISTR_EP_ID_Msk;
    uint8_t type;

    #if DBG >= 1
    SEGGER_RTT_printf(0, "CTR\n");
    #endif

    if (istr & USB_ISTR_DIR_) {
        if (USB->EPR[ep] & USB_EPR_SETUP_Msk) {
            type = USB_TRANSACTION_SETUP;
            usb_ep_read_packet(dev, ep, &dev->ep0.req, USB_SETUP_DATA_SIZE);
        }
        else {
            type = USB_TRANSACTION_OUT;
        }
    }
    else {
        type = USB_TRANSACTION_IN;
        USB_CLR_EPR_CTR_TX(ep);
    }

    if (dev->user_ctr_callback[ep][type]) {
        dev->user_ctr_callback[ep][type] (dev, ep);
    }
    else {
        USB_CLR_EPR_CTR_RX(ep);
    }

}

void usb_handle_event(usb_device *dev) {

    uint16_t istr = USB->ISTR;

    if (istr & USB_ISTR_RESET_) {
        USB_CLR_ISTR_RESET();
        usb_reset(dev);
        return;
    }

    if (istr & USB_ISTR_CTR_) {
        usb_ctr(dev, istr);
    }

    if (istr & USB_ISTR_SUSP_) {
        USB_CLR_ISTR_SUSP();
        /* do whatever */
    }

    if (istr & USB_ISTR_WKUP_) {
        USB_CLR_ISTR_WKUP();
        /* ... */
    }

    if (istr & USB_ISTR_SOF_) {
        USB_CLR_ISTR_SOF();
        /* ... */
    }

}

/* note 1 :  usb_start()
 *
 * trigger re-enumeration:
 * we need to pull down D+ as a signal for the
 * USB host to start FS enumeration.
 * many MCUs will have pupd functionality built
 * into the USB peripheral for this purpose,
 * but not the STM32F103. since there is no
 * embedded pull-up, the "bluepill" PCB has a 
 * fixed 1.5K external pull-up resistor on D+.
 * the host sees this pull-up and assumes a USBFS
 * device that is ready for enumeration, which it 
 * is not. to get around this, we pull PA12 (D+) 
 * low to simulate the device being disconnected 
 * and reconnected, which triggers re-enumeration. 
 * the arbitrary delay ensures the host recognizes 
 * it. the USB peripheral will then take over D+ 
 * and the host will start the enumeration process.
 * https://stackoverflow.com/questions/54939948/stm32-usb-cdc-after-hardware-reset
 * 
 * thanks ST/bluepill :D
 * 
 */ 