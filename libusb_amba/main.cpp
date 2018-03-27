#include <iostream>
#include <amba_usb.h>

using namespace std;

int main(int argc, char *argv[])
{
    amba_usb tmp_dev;
    tmp_dev.usb_init();
    return 0;
}
