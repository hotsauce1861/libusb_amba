#ifndef AMBA_USB_H
#define AMBA_USB_H

#include "libusb.h"

class amba_usb
{
public:
    amba_usb();
    void        usb_init();
    void active_config(struct libusb_device *dev,
                                                     struct libusb_device_handle *handle);

};

#endif // AMBA_USB_H
