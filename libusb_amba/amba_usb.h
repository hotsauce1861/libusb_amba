#ifndef AMBA_USB_H
#define AMBA_USB_H

#include "libusb.h"

class amba_usb
{
public:
    amba_usb();
    ~amba_usb();
    void        usb_init();
    void active_config(struct libusb_device *m_dev,
                       struct libusb_device_handle *handle);

private:
    libusb_device           **m_dev_list;
    libusb_device           *m_dev;
    libusb_device_handle    *m_dev_handle;
    libusb_context          *m_dev_cntx;

};

#endif // AMBA_USB_H
