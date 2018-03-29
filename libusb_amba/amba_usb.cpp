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

char buf[BUF_SIZE] = { 0 };

char cmd_session_start[] = "{\"msg_id\" : 257, \"token\": 0}";
char cmd_get_all_settings[] = "{\"msg_id\" : 3, \"token\": 1}";
char cmd_get_video_res[] = "{\"msg_id\" : 9, \"param\" :\"video_resolution\", \"token\": 1}";

static void LIBUSB_CALL recv_cb_fn(struct libusb_transfer *transfer)
{
    if(transfer->status != LIBUSB_TRANSFER_COMPLETED)
    {
        printf("\nLIBUSB_TRANSFER_COMPLETED");
    }
    printf("\nReceive Callback , Data is %s", buf+16);

}

static void LIBUSB_CALL send_cb_fn(struct libusb_transfer *transfer)
{
    if(transfer->status != LIBUSB_TRANSFER_COMPLETED)
    {
        printf("\nLIBUSB_TRANSFER_COMPLETED");
    }
    printf("\nSend Callback");
}

amba_usb::amba_usb():m_debug_flag(false)
{
    m_dev_snd_cb_fn = send_cb_fn;
    m_dev_rcv_cb_fn = recv_cb_fn;
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

/**
* @brief
*
*/

int amba_usb::usb_async_read_dat(char *buf, int len)
{
    ///async read data operation
    int head_size;
    head_size = m_dev_pktfmt.get_header_size();
    char tmp_buf[BUF_SIZE] = {0};

    m_dev_transfer_recv = libusb_alloc_transfer(1);

    libusb_fill_bulk_transfer(m_dev_transfer_recv,m_dev_handle,
                                   m_dev_param.get_endpoint_in(),
                                   (unsigned char*)buf,
                                   len + head_size,
                                   m_dev_rcv_cb_fn,NULL,0);
    m_dev_transfer_recv->flags |= LIBUSB_TRANSFER_FREE_TRANSFER;
    libusb_submit_transfer(m_dev_transfer_recv);
/*
    if(m_dev_transfer_recv->status == LIBUSB_TRANSFER_COMPLETED){
        printf("\nActual length is %d",m_dev_transfer_recv->actual_length);
        printf("\nReceive buf is %s",tmp_buf);
        memcpy(buf, tmp_buf + head_size, 512);
    }
*/
    return m_dev_transfer_recv->actual_length;
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
                                   m_dev_snd_cb_fn,NULL,0);
    m_dev_transfer->flags |= LIBUSB_TRANSFER_FREE_TRANSFER;
    libusb_submit_transfer(m_dev_transfer);
}

void amba_usb::usb_run()
{
    timeval tv = {1,0};
    int ret,i;

    usb_open();

    struct pollfd pollfd_array[4];
    m_dev_fd_list = (libusb_pollfd**)libusb_get_pollfds(m_dev_cntx);

    for(i=0; m_dev_fd_list[i]!=NULL; i++){
        pollfd_array[i].fd = m_dev_fd_list[i]->fd;
        pollfd_array[i].events = m_dev_fd_list[i]->events;
    }
    printf("\nDevice fd number is %d",i);

    //usb_sync_send_dat(cmd_session_start,strlen(cmd_session_start));
    //usb_async_read_dat(buf,BUF_SIZE);

    usb_opt_user_input();
    while(1){

        libusb_get_next_timeout(m_dev_cntx,&tv);
        ret = poll(pollfd_array,i,NULL);

        if(ret < 0)
        {
            printf("\nGet poll fd failed");
        }else{
            libusb_handle_events_timeout(m_dev_cntx,&tv);
            for(int j = 0; j<i; j++){
                if(pollfd_array[j].fd < 0){
                    //continue;
                }
                if(pollfd_array[j].revents & POLLIN){

                    usb_async_read_dat(buf,BUF_SIZE);
                    printf("\nReceive buf %s", buf);

                }else if(pollfd_array[j].revents & POLLOUT){

                    usb_opt_user_input();
                }
            }
        }
/*
        usb_async_read_dat(buf,BUF_SIZE);
        printf("\nReceive buf %s", buf);
        sleep(1);
*/
    }
}

void amba_usb::usb_opt_user_input()
{
    int opt;
    struct timeval tv = {1,0};
    printf("\n1.cmd_session_start");
    printf("\n2.cmd_get_all_settings");
    printf("\nPlease input option:");
    scanf("%d",&opt);
    switch (opt) {
    case 1:
        libusb_handle_events_timeout(m_dev_cntx,&tv);
        usb_async_send_dat(cmd_session_start,strlen(cmd_session_start));
        break;
    case 2:
        libusb_handle_events_timeout(m_dev_cntx,&tv);
        usb_async_send_dat(cmd_get_all_settings,strlen(cmd_get_all_settings));
        break;
    default:
        break;
    }
}
