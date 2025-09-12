/**********************************************************************************
 ** file         : mouse.c
 ** description  : stm32f103 + paw3395 USB HID mouse
 **
 **
 **********************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "device.h"
#include "rcc.h"
#include "gpio.h"
#include "spim.h"
#include "paw3395.h"
#include "utils.h"
#include "usb.h"
#include "hid.h"
#include "SEGGER_RTT.h"

/* global l/r click states */
volatile uint8_t l_click = 0;
volatile uint8_t r_click = 0;

/* our handle (ptr) to the device alloc'd in `usb.c` */
static usb_device *usb_dev;

const struct usb_device_descriptor device_descriptor = {
    .bLength            = USB_DT_DEVICE_SIZE,
    .bDescriptorType    = USB_DT_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,
    .bMaxPacketSize0    = 64,
    .idVendor           = 0x0483,
    .idProduct          = 0x572B,
    .bcdDevice          = 0x0200,
    .iManufacturer      = 1,
    .iProduct           = 2,
    .iSerialNumber      = 3,
    .bNumConfigurations = 1,
};

struct hid_mouse_report {
    uint8_t buttons;  
    int16_t x;
    int16_t y;
    int16_t wheel;
} __attribute__((packed));

/* 7 byte report:
 * byte 0:
 *   REPORT_COUNT (2), REPORT_SIZE(1) = button 1,2 = 2 bits
 *   REPORT_COUNT (1), REPORT_SIZE(6) = padding = 6 bits
 * bytes 1-6: 
 *   REPORT_COUNT (3), REPORT_SIZE(16) = X, Y, Wheel = 3 * 2 bytes
 */
static const uint8_t hid_mouse_report_descriptor[] = {
    0x05, 0x01,         /* USAGE_PAGE (Generic Desktop)         */
    0x09, 0x02,         /* USAGE (Mouse)                        */
    0xa1, 0x01,         /* COLLECTION (Application)             */
    0x09, 0x01,         /*   USAGE (Pointer)                    */
    0xa1, 0x00,         /*   COLLECTION (Physical)              */
    0x05, 0x09,         /*     USAGE_PAGE (Button)              */
    0x19, 0x01,         /*     USAGE_MINIMUM (Button 1)         */
    0x29, 0x02,         /*     USAGE_MAXIMUM (Button 2)         */
    0x15, 0x00,         /*     LOGICAL_MINIMUM (0)              */
    0x25, 0x01,         /*     LOGICAL_MAXIMUM (1)              */
    0x95, 0x02,         /*     REPORT_COUNT (2)                 */
    0x75, 0x01,         /*     REPORT_SIZE (1)                  */
    0x81, 0x02,         /*     INPUT (Data,Var,Abs)             */
    0x95, 0x01,         /*     REPORT_COUNT (1)                 */
    0x75, 0x06,         /*     REPORT_SIZE (6)                  */
    0x81, 0x01,         /*     INPUT (Cnst,Ary,Abs)             */
    0x05, 0x01,         /*     USAGE_PAGE (Generic Desktop)     */
    0x09, 0x30,         /*     USAGE (X)                        */
    0x09, 0x31,         /*     USAGE (Y)                        */
    0x09, 0x38,         /*     USAGE (Wheel)                    */
    0x16, 0x01, 0x80,   /*     LOGICAL_MINIMUM (-32767)         */
    0x26, 0xff, 0x7f,   /*     LOGICAL_MAXIMUM (32767)          */
    0x95, 0x03,         /*     REPORT_COUNT (3)                 */
    0x75, 0x10,         /*     REPORT_SIZE (16)                 */
    0x81, 0x06,         /*     INPUT (Data,Var,Rel)             */
    0xc0,               /*   END_COLLECTION                     */
    0x09, 0x3c,         /*   USAGE (Motion Wakeup)              */
    0xc0                /* END_COLLECTION                       */
};

struct config_block {
    struct usb_configuration_descriptor config;
    struct usb_interface_descriptor     if0;
    struct usb_hid_descriptor           if0_hid;
    struct usb_endpoint_descriptor      if0_hid_ep;
} __attribute__((packed));

struct config_block hid_mouse_cfg_block = {
    
    .config = {
        .bLength                = USB_DT_CONFIGURATION_SIZE,
        .bDescriptorType        = USB_DT_CONFIGURATION,
        .wTotalLength           = sizeof(struct usb_configuration_descriptor) +
                                  sizeof(struct usb_interface_descriptor) +
                                  sizeof(struct usb_hid_descriptor) +
                                  sizeof(struct usb_endpoint_descriptor),
        .bNumInterfaces         = 1,
        .bConfigurationValue    = 1,
        .iConfiguration         = 0,
        .bmAttributes           = 0x80,
        .bMaxPower              = 0x32,
    },

    .if0 = {
        .bLength                = USB_DT_INTERFACE_SIZE,
        .bDescriptorType        = USB_DT_INTERFACE,
        .bInterfaceNumber       = 0,
        .bAlternateSetting      = 0,
        .bNumEndpoints          = 1,
        .bInterfaceClass        = USB_CLASS_HID,
        .bInterfaceSubClass     = USB_HID_SUBCLASS_NO,
        .bInterfaceProtocol     = USB_HID_INTERFACE_PROTOCOL_NONE,
        .iInterface             = 0,
    },

    .if0_hid = {
        .bLength                 = sizeof(struct usb_hid_descriptor),
        .bDescriptorType         = USB_HID_DT_HID,
        .bcdHID                  = 0x0111,
        .bCountryCode            = 0,
        .bNumDescriptors         = 1,
        .bReportDescriptorType   = USB_HID_DT_REPORT,
        .wReportDescriptorLength = sizeof(hid_mouse_report_descriptor),
    },

    .if0_hid_ep = {
        .bLength                = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType        = USB_DT_ENDPOINT,
        .bEndpointAddress       = 0x81,
        .bmAttributes           = USB_EP_ATTR_INTERRUPT,
        .wMaxPacketSize         = sizeof(struct hid_mouse_report),
        .bInterval              = 0x1,
    }

};

const struct usb_string_descriptor str_langid = {
    .bLength            = 4,
    .bDescriptorType    = USB_DT_STRING,
    .wString            = { USB_LANGID_EN_US },
};

const struct usb_string_descriptor str_mfr = {
    /* bLength = bLength + bDescriptorType + (ASCII chars * 2)
     *         =    1    +        1        + (      9     * 2) 
     *         =   20
     */
    .bLength            = 20,
    .bDescriptorType    = USB_DT_STRING,
    .wString            = {
        'H','i','i','r','i',' ','C','o','.'
    },
};

const struct usb_string_descriptor str_product = {
    .bLength            = 20,
    .bDescriptorType    = USB_DT_STRING,
    .wString            = {
        'H','I','D',' ','M','o','u','s','e'
    }
};

const struct usb_string_descriptor str_serial = {
    .bLength            = 10,
    .bDescriptorType    = USB_DT_STRING,
    .wString            = {
        '1', '3', '3', '7'
    }
};

const struct usb_string_descriptor * const strings[] = {
    &str_langid,
    &str_mfr,
    &str_product,
    &str_serial,
};

static void clock_setup(void) {

    set_sysclk_72mhz();
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN_;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN_;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN_;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN_;

}

static void gpio_setup(void) {

    /* PA10 (L_NC): input with pull-up */
    GPIOA->CRH = (GPIOA->CRH & ~GPIO_CRH_CNFMODE10_Msk) | (GPIO_CNFMODE_INPUT_PUPD << GPIO_CRH_CNFMODE10_Shft);
    GPIOA->ODR |= GPIO10;

    /* PA9 (L_NO): input with pull-up */
    GPIOA->CRH = (GPIOA->CRH & ~GPIO_CRH_CNFMODE9_Msk)  | (GPIO_CNFMODE_INPUT_PUPD << GPIO_CRH_CNFMODE9_Shft);
    GPIOA->ODR |= GPIO9;

    /* PB12 (R_NC): input with pull-up */
    GPIOB->CRH = (GPIOB->CRH & ~GPIO_CRH_CNFMODE12_Msk) | (GPIO_CNFMODE_INPUT_PUPD << GPIO_CRH_CNFMODE12_Shft);
    GPIOB->ODR |= GPIO12;

    /* PA8 (R_NO): input with pull-up */
    GPIOA->CRH = (GPIOA->CRH & ~GPIO_CRH_CNFMODE8_Msk)  | (GPIO_CNFMODE_INPUT_PUPD << GPIO_CRH_CNFMODE8_Shft);
    GPIOA->ODR |= GPIO8;

    /* rm0008 9.1.11 table 25
     * SPI GPIO configurations */
    
    /* PA4 (SPI1_CS) -> GP pupd 50MHz */
    GPIOA->CRL = (GPIOA->CRL & ~GPIO_CRL_CNFMODE4_Msk) | (GPIO_CNFMODE_OUTPUT_GP_PUSHPULL_50MHZ << GPIO_CRL_CNFMODE4_Shft);
    gpio_set(GPIOA, GPIO4);

    /* PA5 (SPI1_SCK) -> AF pupd 50MHz */
    GPIOA->CRL = (GPIOA->CRL & ~GPIO_CRL_CNFMODE5_Msk) | (GPIO_CNFMODE_OUTPUT_AF_PUSHPULL_50MHZ << GPIO_CRL_CNFMODE5_Shft);

    /* PA6 (SPI1_MISO) -> input floating */
    GPIOA->CRL = (GPIOA->CRL & ~GPIO_CRL_CNFMODE6_Msk) | (GPIO_CNFMODE_INPUT_FLOAT << GPIO_CRL_CNFMODE6_Shft);

    /* PA7 (SPI1_MOSI) -> AF pupd 50MHz  */
    GPIOA->CRL = (GPIOA->CRL & ~GPIO_CRL_CNFMODE7_Msk) | (GPIO_CNFMODE_OUTPUT_AF_PUSHPULL_50MHZ << GPIO_CRL_CNFMODE7_Shft);

}

static void tim_setup(void) {

    /* 72MHz / 72 = 1MHz */
    TIM2->PSC = 71;

    /* generate update event: apply PSC */
    TIM2->EGR |= TIM_EGR_UG_;

    /* enable TIM2 counter */
    TIM2->CR1 |= TIM_CR1_CEN_;

}

static void exti_setup(void) {
    
    /* map EXTI lines 8-10 to PA8-10 */
    AFIO->EXTICR[2] = (AFIO->EXTICR[2] & ~(AFIO_EXTICR3_EXTI8_Msk
                                          | AFIO_EXTICR3_EXTI9_Msk
                                          | AFIO_EXTICR3_EXTI10_Msk))
                                        | (AFIO_EXTICR3_EXTI8_PA8 
                                          | AFIO_EXTICR3_EXTI9_PA9
                                          | AFIO_EXTICR3_EXTI10_PA10);

    /* map EXTI line 12 to PB12 */
    AFIO->EXTICR[3] = (AFIO->EXTICR[3] & ~AFIO_EXTICR4_EXTI12_Msk) | (AFIO_EXTICR4_EXTI12_PB12);

    /* unmask line 9 (L_NO) for now, keep line 10 masked (L_NC) */
    EXTI->IMR |=  EXTI9;
    EXTI->IMR &= ~EXTI10;

    /* unmask line 8 (R_NO) for now, keep line 12 masked (R_NC) */
    EXTI->IMR |=  EXTI8;
    EXTI->IMR &= ~EXTI12;

    /* enable falling-edge triggers */
    EXTI->FTSR |= EXTI8;
    EXTI->FTSR |= EXTI9;
    EXTI->FTSR |= EXTI10;
    EXTI->FTSR |= EXTI12;

    /* clear any potential spurious pending bits */
    EXTI->PR = EXTI8 | EXTI9 | EXTI10 | EXTI12;

    /* enable interrupts at NVIC level */
    NVIC->ISER[NVIC_EXTI9_5_IRQ / 32] = (1 << (NVIC_EXTI9_5_IRQ % 32));
    NVIC->ISER[NVIC_EXTI15_10_IRQ / 32] = (1 << (NVIC_EXTI15_10_IRQ % 32));

}

static void spi_setup(void) {

    /* at reset, configured as 2-line unidirectional full-duplex.
     * this is standard 4-wire SPI, so we'll keep it this way */

    /* otherwise, let's set our communication parameters */
    SPI1->CR1 = (SPI1->CR1 & ~SPI_CR1_BR_Msk) | (SPI_CR1_BR_FPCLK_DIV8);
    SPI1->CR1 |= SPI_CR1_CPOL_;
    SPI1->CR1 |= SPI_CR1_CPHA_;
    SPI1->CR1 &= ~SPI_CR1_DFF_;
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST_;

    /* enable master mode */
    SPI1->CR1 |= SPI_CR1_MSTR_;

    /* manage CS manually in software */
    SPI1->CR1 |= SPI_CR1_SSM_;
    SPI1->CR2 &= ~SPI_CR2_SSOE_;

    /* for some reason, must set NSS high when manually controlling
     * CS via GPIO, or else MSTR and SPE will be cleared (rm0008 25.3.3).
     * the SSI bit is provided for this, it's meaningless unless SSM is set */
    SPI1->CR1 |= SPI_CR1_SSI_;

    /* enable peripheral */
    SPI1->CR1 |= SPI_CR1_SPE_;

}

static enum usb_req_result
handle_hid_get_report_descriptor(usb_device *dev, struct usb_setup_data *req, uint8_t **buf, 
                                 uint16_t *len, usb_ep0_req_complete_callback *cb) {
    (void)dev;
    (void)cb;

    if ((req->bmRequestType != 0b10000001) || (req->bRequest != USB_REQ_GET_DESCRIPTOR)
        || (req->wValue != (USB_HID_DT_REPORT << 8))) {
        return USB_REQ_DEFER; /* defer handling to std handlers or subsequent user cb */
    }

    /* put hid report descriptor in usb buffer */
    *buf = (uint8_t *)hid_mouse_report_descriptor;
    *len = sizeof(hid_mouse_report_descriptor);

    return USB_REQ_HANDLED;
}

static void send_hid_report(usb_device *dev, uint8_t ep) {

    (void)ep;
    struct hid_mouse_report report  = {0};
    uint8_t paw_data[BURST_SIZE]    = {0};
    int16_t dx = 0, dy = 0;

    paw_motion_burst(paw_data, sizeof(paw_data));
    dx = (int16_t) ( (paw_data[3] << 8) | (paw_data[2] << 0) );
    dy = (int16_t) ( (paw_data[5] << 8) | (paw_data[4] << 0) );

    report.buttons  = ((r_click << 1) | (l_click << 0));
    report.x        = dx;
    report.y        = dy;

    usb_ep_write_packet(dev, 0x81, &report, sizeof(report));

}

static void hid_set_configuration(usb_device *dev, uint16_t wValue) {

    (void)wValue;

    usb_setup_ep(dev, 0x81, USB_EP_ATTR_INTERRUPT, sizeof(struct hid_mouse_report), send_hid_report);

    usb_register_ep0_req_handler(dev, 
        USB_REQ_TYPE_IN        | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
        USB_REQ_TYPE_DIRECTION | USB_REQ_TYPE_TYPE     | USB_REQ_TYPE_RECIPIENT, 
        handle_hid_get_report_descriptor);

    /* fill ep1 tx buffer with first report; start chain of CTR IN events */
    send_hid_report(dev, 0x81);

}

int main(void) {

    clock_setup();
    gpio_setup();
    tim_setup();
    exti_setup();
    spi_setup();

    paw_init();
    paw_set_dpi(800);

    /* receive usb device handler */
    usb_dev = usb_init(&device_descriptor, &hid_mouse_cfg_block.config, strings, 4);

    /* register the func that will run when the host sends the `set_configuration` request */
    usb_register_set_config_callback(usb_dev, hid_set_configuration);

    /* enable peripheral, start enumeration */
    usb_start(usb_dev);

    for (;;) {
        usb_handle_event(usb_dev);
    }

}

/* 
 * SPDT 0-latency debounce logic (SR-Latch emulation) 
 */

void exti9_5_isr(void) {

    if (EXTI->PR & EXTI9) {
        
        /* disable PA9 (L_NO), enable PA10 (L_NC) */
        EXTI->IMR &= ~EXTI9;
        EXTI->IMR |=  EXTI10;

        /* clear pending bits */
        EXTI->PR = EXTI9 | EXTI10;

        l_click = 1;

    }

    if (EXTI->PR & EXTI8) {

        /* disable PA8 (R_NO), enable PB12 (R_NC) */
        EXTI->IMR &= ~EXTI8;
        EXTI->IMR |=  EXTI12;

        /* clear pending bits */
        EXTI->PR = EXTI8 | EXTI12;

        r_click = 1;

    }

}

void exti15_10_isr(void) {

    if (EXTI->PR & EXTI10) {
        
        /* disable PA10 (L_NC), enable PA9 (L_NO) */
        EXTI->IMR &= ~EXTI10;
        EXTI->IMR |=  EXTI9;

        /* clear pending bits */
        EXTI->PR = EXTI9 | EXTI10;

        l_click = 0;

    }

    if (EXTI->PR & EXTI12) {

        /* disable PB12 (R_NC), enable PA8 (R_NO) */
        EXTI->IMR &= ~EXTI12;
        EXTI->IMR |=  EXTI8;

        /* clear pending bits */
        EXTI->PR = EXTI8 | EXTI12;

        r_click = 0;

    }

}
