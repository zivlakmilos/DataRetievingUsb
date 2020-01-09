#ifndef _USB_H_
#define _USB_H_

#include <vector>
#include <libudev.h>

#include "UsbDevice.h"

class Usb
{
public:
    Usb(void);
    virtual ~Usb(void);

    std::vector<UsbDevice> getBlockDeviceList(void);

    bool isBlockDeviceExists(const std::string &syspath);
    UsbDevice getBlockDevice(const std::string &syspath);

private:
    struct udev *m_udev;
};

#endif // _USB_H_
