#include "amba_usb.h"
#include <stdio.h>
#include <iostream>

#ifdef WIN32
#include <windows.h>
#elif
#include <unistd.h>
#include <pthread.h>
#endif

char cmd_session_start[] = "{\"msg_id\" : 257, \"token\": 0}";
char cmd_get_all_settings[] = "{\"msg_id\" : 3, \"token\": 1}";
char cmd_get_video_res[] = "{\"msg_id\" : 9, \"param\" :\"video_resolution\", \"token\": 1}";


amba_usb::amba_usb():m_debug_flag(false)
{

}

amba_usb::~amba_usb()
{
    libusb_close(m_dev_handle);
    libusb_release_interface(m_dev_handle,0);
    std::cout << "exit task" << std::endl;
}

void amba_usb::usb_dump()
{  

}

int amba_usb::usb_open()
{
    int ret;
    ret = libusb_init(&m_dev_cntx);
    if(ret < 0)
    {
        return -1;
    }
    if(m_debug_flag){
        libusb_set_debug(m_dev_cntx,LIBUSB_LOG_LEVEL_DEBUG);
    }
    m_dev_handle = libusb_open_device_with_vid_pid(m_dev_cntx,
                                                   m_dev_param.get_vid(),
                                                   m_dev_param.get_pid());

    m_dev = libusb_get_device(m_dev_handle);
    ///interface number meaning?
    if(libusb_kernel_driver_active(m_dev_handle, 0) == 1)
    {
        printf("\nKernel Driver Active");
        if(libusb_detach_kernel_driver(m_dev_handle, 0) == 0)
            printf("\nKernel Driver Detached!");
        else
        {
            printf("\nCouldn't detach kernel driver!\n");
            //libusb_free_device_list(devs,1);
            libusb_close(m_dev_handle);
            return -1;
        }
    }

    libusb_claim_interface(m_dev_handle, 0);

    return 0;
}

int amba_usb::usb_sync_send_dat(char *buf, int len)
{
    int ret,act_len,head_size;
    head_size = m_dev_pktfmt.get_header_size();
    char tmp_buf[BUF_SIZE] = {0};
    memcpy(tmp_buf + head_size, buf, len);
    ret = libusb_bulk_transfer(m_dev_handle,m_dev_param.get_endpoint_out(),
                               (unsigned char *)tmp_buf,
                               len + head_size,
                               &act_len,
                               NULL);
}

int amba_usb::usb_sync_read_dat(char *buf, int len)
{
    int ret,act_len;
    ret = libusb_bulk_transfer(m_dev_handle,m_dev_param.get_endpoint_in(),
                               (unsigned char*)buf,
                               len,
                               &act_len,
                               NULL);
}

void amba_usb::usb_run()
{
    char buf[BUF_SIZE] = { 0 };
    usb_open();
    usb_sync_send_dat(cmd_session_start,strlen(cmd_session_start));
    usb_sync_read_dat(buf,BUF_SIZE);
    printf("\nReceive buf %s", buf);
}
