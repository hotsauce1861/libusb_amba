#ifndef AMBA_USB_H
#define AMBA_USB_H

#include "libusb.h"

#define ENDP_IN     0x01
#define ENDP_OUT    0x82

class amba_usb
{
public:
    amba_usb();
    ~amba_usb();
    void                    usb_init();
    void                    active_config(struct libusb_device *m_dev,struct libusb_device_handle *handle);

    void                    set_vid(int _vid){this->m_vid = _vid;}
    void                    set_pid(int _pid){this->m_pid = _pid;}
    int                     get_vid(){return m_vid;}
    int                     get_pid(){return m_pid;}

private:
    libusb_device           **m_dev_list;
    libusb_device           *m_dev;
    libusb_device_handle    *m_dev_handle;
    libusb_context          *m_dev_cntx;
    int                     m_vid;
    int                     m_pid;
};

#endif // AMBA_USB_H
