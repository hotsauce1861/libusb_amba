#include "amba_usb.h"
#include <stdio.h>
#include <iostream>
#include <pthread.h>

#ifdef WIN32

#endif

int interface_ref = 0;
int alt_interface,interface_number;

struct libusb_endpoint_descriptor;
amba_usb::amba_usb()
{

}

void amba_usb::active_config(struct libusb_device *dev,struct libusb_device_handle *handle)
{
    struct libusb_device_handle *hDevice_req;
    struct libusb_config_descriptor *config;
    struct libusb_endpoint_descriptor *endpoint;
    int altsetting_index,interface_index=0,ret_active;
    int i,ret_print;

    hDevice_req = handle;

    ret_active = libusb_get_active_config_descriptor(dev,&config);
    //ret_print = print_configuration(hDevice_req,config);

    for(interface_index=0;interface_index<config->bNumInterfaces;interface_index++)
    {
        const struct libusb_interface *iface = &config->interface[interface_index];
        for(altsetting_index=0;altsetting_index<iface->num_altsetting;altsetting_index++)
        {
            const struct libusb_interface_descriptor *altsetting = &iface->altsetting[altsetting_index];

            int endpoint_index;
            for(endpoint_index=0;endpoint_index<altsetting->bNumEndpoints;endpoint_index++)
            {
                //struct libusb_endpoint_desriptor *ep = &altsetting->endpoint[endpoint_index];
                //endpoint = &altsetting->endpoint[endpoint_index];
                alt_interface = altsetting->bAlternateSetting;
                interface_number = altsetting->bInterfaceNumber;
            }

            printf("\nEndPoint Descriptors: ");
            printf("\n\tSize of EndPoint Descriptor : %d",endpoint->bLength);
            printf("\n\tType of Descriptor : %d",endpoint->bDescriptorType);
            printf("\n\tEndpoint Address : 0x0%x",endpoint->bEndpointAddress);
            printf("\n\tMaximum Packet Size: %x",endpoint->wMaxPacketSize);
            printf("\n\tAttributes applied to Endpoint: %d",endpoint->bmAttributes);
            printf("\n\tInterval for Polling for data Tranfer : %d\n",endpoint->bInterval);
        }
    }
    libusb_free_config_descriptor(NULL);
    //return endpoint;
}

void amba_usb::usb_init()
{
    //char buf[] = "{\"msg_id\":3,\"token\":0 }";    
    char strToDevice[] = "{\"rval\" : 0,\"msg_id\" : 257, \"type\": \"free\", \"param\" : \"1048576, 1048576, 1048576, 1230\"}";
    char cmd_session_start[] = "{\"msg_id\" : 257, \"token\": 0}";
    char cmd_get_all_settings[] = "{\"msg_id\" : 3, \"token\": 1}";
    char cmd_get_video_res[] = "{\"msg_id\" : 9, \"param\" :\"video_resolution\", \"token\": 1}";

    unsigned char buf[512] = { 0 };
    unsigned char* p_buf = &buf[0];
    memmove(buf + 8,buf,sizeof(cmd_session_start));
    libusb_device **list;
    libusb_device *dev;
    libusb_device_handle *dev_handle;
    libusb_context *dev_cntx;
    int ret, count, length;

    ret = libusb_init(&dev_cntx);
    libusb_set_debug(dev_cntx,LIBUSB_LOG_LEVEL_DEBUG);

    if(ret < 0)
    {
        printf("init usb failed");
        exit(1);
    }
    count = libusb_get_device_list(NULL,&list);
    if(count < 0){
        printf("get device list failed");
        exit(2);
    }
    for(int i=0; i<count; i++)
    {
        dev = list[i];

    }
    dev_handle = libusb_open_device_with_vid_pid(dev_cntx,0x4255,0x0001);
    dev = libusb_get_device(dev_handle);
#define DISPLAY_DECS
#ifdef  DISPLAY_DECS
    struct libusb_interface_descriptor i_desc;
    struct libusb_config_descriptor *c_desc;
    struct libusb_device_descriptor d_desc;

    libusb_get_config_descriptor(dev,0,&c_desc);
    libusb_get_device_descriptor(dev,&d_desc);


    printf("SerialNumber is %d \n", d_desc.iSerialNumber);
    printf("%04x:%04x\n", d_desc.idVendor,d_desc.idProduct);
    printf("\nDevice Descriptors: ");
    printf("\n\tSerial Number : %x",d_desc.iSerialNumber);
    printf("\n\tSize of Device Descriptor : %d",d_desc.bLength);
    printf("\n\tType of Descriptor : %d",d_desc.bDescriptorType);
    printf("\n\tUSB Specification Release Number : %d",d_desc.bcdUSB);
    printf("\n\tDevice Release Number : %d",d_desc.bcdDevice);
    printf("\n\tDevice Class : %d",d_desc.bDeviceClass);
    printf("\n\tDevice Sub-Class : %d",d_desc.bDeviceSubClass);
    printf("\n\tDevice Protocol : %d",d_desc.bDeviceProtocol);
    printf("\n\tMax. Packet Size : %d",d_desc.bMaxPacketSize0);
    printf("\n\tNo. of Configuraions : %d\n",d_desc.bNumConfigurations);

#endif

    libusb_set_auto_detach_kernel_driver(dev_handle,1);

    if(libusb_kernel_driver_active(dev_handle, 0) == 1)
    {
        printf("\nKernel Driver Active");
        if(libusb_detach_kernel_driver(dev_handle, 0) == 0)
            printf("\nKernel Driver Detached!");
        else
        {
            printf("\nCouldn't detach kernel driver!\n");
            //libusb_free_device_list(devs,1);
            libusb_close(dev_handle);
            return;
        }
    }

    //libusb_claim_interface(dev_handle, 0);
    libusb_claim_interface(dev_handle, 0);

    //active_config(dev,dev_handle);
    Sleep(10000);

    ret = libusb_bulk_transfer(dev_handle,0x82,p_buf,sizeof(buf),&length,1000);

    printf("length is %d\n",length);
    printf("size of buf is %d\n",sizeof(buf));

    if(ret == 0 && length == sizeof(buf)){
        printf("bulk transfer success\n");
    }else{
        printf("bulk transfer failed\n");
    }

    while (1)
    {
        int input = 0;

        printf("Command List:\n");
        printf("0.session start\n");
        printf("1.get all setting\n");
        printf("2.get video res\n");

        scanf("%d", &input);

        switch (input)
        {
        case 0://session start
            memmove(buf + 8,buf,sizeof(cmd_session_start));
            p_buf = &buf[0];
            libusb_bulk_transfer(dev_handle,0x82,buf,sizeof(buf),&length,1000);
            break;
        case 1://set
            memmove(buf + 8,buf,sizeof(cmd_get_all_settings));
            p_buf = &buf[0];
            libusb_bulk_transfer(dev_handle,0x82,buf,sizeof(buf),&length,1000);
            break;
        case 2:
            memmove(buf + 8,buf,sizeof(cmd_get_video_res));
            p_buf = &buf[0];
            libusb_bulk_transfer(dev_handle,0x82,buf,sizeof(buf),&length,1000);
        case 10://quit
            break;

        }
    }
}
