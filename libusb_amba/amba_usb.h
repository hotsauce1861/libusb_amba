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
    int                     usb_async_send_dat(char *buf, int len);
    int                     usb_async_read_dat(char *buf, int len);
    void                    usb_set_async_mode();
    void                    usb_set_sync_mode();
    void                    usb_run();
    void                    usb_debug(bool _flag){  m_debug_flag = _flag; }
    void                    set_snd_cb_fn(libusb_transfer_cb_fn tmp_cb_fn);
    void                    set_rcv_cb_fn(libusb_transfer_cb_fn tmp_cb_fn);
    void                    usb_opt_user_input();


private:
    amba_param              m_dev_param;
    amba_pktfmt             m_dev_pktfmt;

    libusb_device           **m_dev_list;
    libusb_device           *m_dev;
    libusb_device_handle    *m_dev_handle;
    libusb_context          *m_dev_cntx;
    libusb_pollfd           *m_dev_fd;
    libusb_pollfd           **m_dev_fd_list;
    libusb_transfer         *m_dev_transfer;
    libusb_transfer         *m_dev_transfer_recv;

    libusb_transfer_cb_fn   m_dev_snd_cb_fn;
    libusb_transfer_cb_fn   m_dev_rcv_cb_fn;

    bool                    m_debug_flag;
};

#endif // AMBA_USB_H
