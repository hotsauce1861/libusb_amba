#include "amba_usb.h"
#include <stdio.h>
#include <string.h>
#include <iostream>


#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
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
    char tmp_buf[BUF_SIZE] = { 0 };
    int ret,act_len,head_size;
    head_size = m_dev_pktfmt.get_header_size();
    ret = libusb_bulk_transfer(m_dev_handle,m_dev_param.get_endpoint_in(),
                               (unsigned char*)tmp_buf,
                               len + head_size,
                               &act_len,
                               NULL);
    memcpy(buf, tmp_buf + head_size, act_len - head_size);
}

int amba_usb::usb_async_read_dat(char *buf, int len)
{
    ///async read data operation
    int ret,head_size;
    head_size = m_dev_pktfmt.get_header_size();
    char tmp_buf[BUF_SIZE] = {0};

    //m_dev_transfer = libusb_alloc_transfer(0);

    libusb_fill_bulk_transfer(m_dev_transfer,m_dev_handle,
                                   m_dev_param.get_endpoint_out(),
                                   (unsigned char*)tmp_buf,
                                   len + head_size,
                                   NULL,NULL,NULL);
    libusb_submit_transfer(m_dev_transfer);
    printf("\n\n\nActual length is %d\n\n\n",m_dev_transfer->actual_length);
    memcpy(buf, tmp_buf + head_size, m_dev_transfer->actual_length - head_size);
    if(m_dev_transfer->status == LIBUSB_TRANSFER_COMPLETED){
        memcpy(buf, tmp_buf + head_size, m_dev_transfer->actual_length - head_size);
    }

}

int amba_usb::usb_async_send_dat(char *buf, int len)
{
    int ret,act_len,head_size;
    head_size = m_dev_pktfmt.get_header_size();
    char tmp_buf[BUF_SIZE] = {0};
    memcpy(tmp_buf + head_size, buf, len);

    m_dev_transfer = libusb_alloc_transfer(0);

    libusb_fill_bulk_transfer(m_dev_transfer,m_dev_handle,
                                   m_dev_param.get_endpoint_out(),
                                   (unsigned char*)tmp_buf,
                                   len + head_size,
                                   NULL,NULL,5000);
    libusb_submit_transfer(m_dev_transfer);
}

void amba_usb::usb_run()
{
    int ret,i;
    char buf[BUF_SIZE] = { 0 };
    usb_open();

    struct pollfd pollfd_array[20];
    m_dev_fd_list = (libusb_pollfd**)libusb_get_pollfds(m_dev_cntx);

    for(i=0; m_dev_fd_list[i]!=NULL; i++){
        pollfd_array[i].fd = m_dev_fd_list[i]->fd;
        pollfd_array[i].events = m_dev_fd_list[i]->events;
    }
    printf("\nDevice fd number is %d",i);
    m_dev_transfer = libusb_alloc_transfer(0);
    //usb_sync_send_dat(cmd_session_start,strlen(cmd_session_start));
    //usb_async_read_dat(buf,BUF_SIZE);
    usb_async_send_dat(cmd_session_start,strlen(cmd_session_start));
    while(1){

        ret = poll(pollfd_array,i,5000);

        if(ret < 0)
        {

        }else{

            for(int j = 0;i<i;j++){
                if(pollfd_array[j].revents & POLLIN){
                    usb_async_read_dat(buf,BUF_SIZE);
                    printf("\nReceive buf %s \n", buf);
                }
            }
            //timeval tv = {1,0};
            //usb_async_read_dat(buf,BUF_SIZE);
            //libusb_handle_events_timeout(m_dev_cntx,&tv);
            //printf("\nReceive buf %s \n", buf);
        }      
        sleep(1);
    }
}


