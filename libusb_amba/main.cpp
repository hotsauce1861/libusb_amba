#include <iostream>
#include <amba_usb.h>

using namespace std;

int main(int argc, char *argv[])
{
    amba_usb tmp_dev;
    tmp_dev.usb_debug(true);
    tmp_dev.usb_run();
    return 0;
}
