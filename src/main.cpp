#include <iostream>

#include "Usb.h"

int main(int argc, char *argv[])
{

    Usb usb;

    std::vector<UsbDevice> deviceList = usb.getBlockDeviceList();

    for (auto device : deviceList)
    {
        std::cout << "DEVNODE: " << device.getDevNode() << std::endl
                  << "DEVTYPE: " << device.getDevType() << std::endl
                  << "SYSPATH: " << device.getSysPath() << std::endl << std::endl;
    }

    return 0;
}
