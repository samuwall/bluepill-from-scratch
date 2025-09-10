/**********************************************************************************
 ** file         : usbhid.c
 ** description  : stm32f103 USB HID mouse
 **
 **
 **********************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "device.h"
#include "rcc.h"
#include "utils.h"
#include "usb.h"
#include "hid.h"

#include "SEGGER_RTT.h"

static void hid_set_configuration(usb_device *dev, uint16_t wValue);

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
        .bInterval              = 0x30,
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
}

static void set_polling_rate(usb_device *dev, struct usb_setup_data *req) {

    (void)dev;

    uint8_t bInterval = req->wIndex;
    hid_mouse_cfg_block.if0_hid_ep.bInterval = bInterval;

    #if DBG >= 1
    SEGGER_RTT_printf(0, "\nRESETTING DEVICE WITH NEW POLLING RATE!\nbInterval = %d\n\n", 
                         bInterval);
    #endif

    usb_stop(usb_dev);
    usb_dev = usb_init(&device_descriptor, &hid_mouse_cfg_block.config, strings, 4);
    usb_register_set_config_callback(usb_dev, hid_set_configuration);
    usb_start(usb_dev);

}

static enum usb_req_result
handle_set_mousesettings(usb_device *dev, struct usb_setup_data *req, uint8_t **buf, 
                         uint16_t *len, usb_ep0_req_complete_callback *cb) {
    (void)dev;
    (void)buf;
    (void)len;

    /* custom 'vendor-specific' request for communicating DPI/pollrate requests 
     */
    if ((req->bmRequestType != 0b01000000) || (req->bRequest != 0x01)) {
        return USB_REQ_DEFER;
    }

    if (req->wValue > 0) {
        #if DBG >= 1
        SEGGER_RTT_printf(0, "set dpi: %d\n", req->wValue);
        #endif
    }
    if (req->wIndex > 0) {
        *cb = set_polling_rate;
    }

    return USB_REQ_HANDLED;
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
    static struct hid_mouse_report report = {0};
    static int cnt = 0;
    static int dir = 1;

    cnt += dir;
    if (cnt > 5) {
        dir = -dir;
    }
    if (cnt < -5) {
        dir = -dir;
    }

    report.x = dir;
    usb_ep_write_packet(dev, 0x81, &report, sizeof(report));

}

static void hid_set_configuration(usb_device *dev, uint16_t wValue) {

    (void)wValue;

    usb_setup_ep(dev, 0x81, USB_EP_ATTR_INTERRUPT, sizeof(struct hid_mouse_report), send_hid_report);

    usb_register_ep0_req_handler(dev, 
        USB_REQ_TYPE_IN        | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
        USB_REQ_TYPE_DIRECTION | USB_REQ_TYPE_TYPE     | USB_REQ_TYPE_RECIPIENT, 
        handle_hid_get_report_descriptor);

    usb_register_ep0_req_handler(dev, 
        USB_REQ_TYPE_OUT       | USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_DEVICE,
        USB_REQ_TYPE_DIRECTION | USB_REQ_TYPE_TYPE   | USB_REQ_TYPE_RECIPIENT, 
        handle_set_mousesettings);

    /* fill ep1 tx buffer with first report; start chain of CTR IN events */
    send_hid_report(dev, 0x81);
    
}

int main(void) {

    clock_setup();

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
