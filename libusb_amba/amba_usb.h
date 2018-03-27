#ifndef AMBA_USB_H
#define AMBA_USB_H

#include "libusb.h"
#include "amba_param.h"
#include "amba_pktfmt.h"

class amba_param;
class amba_pktfmt;

class amba_usb
{
public:
    #define     BUF_SIZE    1024
    amba_usb();
    ~amba_usb();
    void                    usb_dump();
    int                     usb_open();
    int                     usb_sync_send_dat(char *buf, int len);
    int                     usb_sync_read_dat(char *buf, int len);
    void                    usb_run();
    void                    usb_debug(bool _flag){  m_debug_flag = _flag; }
private:
    amba_param              m_dev_param;
    amba_pktfmt             m_dev_pktfmt;

    libusb_device           **m_dev_list;
    libusb_device           *m_dev;
    libusb_device_handle    *m_dev_handle;
    libusb_context          *m_dev_cntx;
    bool                    m_debug_flag;
};

#endif // AMBA_USB_H
