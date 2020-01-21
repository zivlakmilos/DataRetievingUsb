#include <iostream>

#include "Log.h"
#include "Settings.h"
#include "Usb.h"

int main(int argc, char *argv[])
{
    LOG_INFO("Test");

    Usb usb;

    std::vector<UsbDevice> deviceList = usb.getBlockDeviceList();

    for (auto device : deviceList)
    {
        if (device.getDevType() == "disk" || device.getDevNode().find("/dev/sda") != std::string::npos)
        {
            continue;
        }

        device.mount("/mnt");
        device.umount();

        std::cout << "DEVNODE: " << device.getDevNode() << std::endl
                  << "DEVTYPE: " << device.getDevType() << std::endl
                  << "SYSPATH: " << device.getSysPath() << std::endl << std::endl;
    }

    return 0;
}
