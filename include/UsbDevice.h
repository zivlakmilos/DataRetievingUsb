#ifndef _USB_DEVICE_H_
#define _USB_DEVICE_H_

#include <string>
#include <libudev.h>

class UsbDevice
{
public:
    UsbDevice(struct udev_device *dev);
    UsbDevice(const UsbDevice &device);
    virtual ~UsbDevice(void);

    inline std::string getDevNode(void) { return std::string(udev_device_get_devnode(m_dev)); }
    inline std::string getDevType(void) { return std::string(udev_device_get_devtype(m_dev)); }
    inline std::string getSysPath(void) { return std::string(udev_device_get_syspath(m_dev)); }

private:
    struct udev_device *m_dev;
};

#endif // _USB_DEVICE_H_
