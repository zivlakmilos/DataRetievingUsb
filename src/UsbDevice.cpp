#include "UsbDevice.h"

UsbDevice::UsbDevice(struct udev_device *dev)
    : m_dev(dev)
{
}

UsbDevice::UsbDevice(const UsbDevice &device)
{
    m_dev = udev_device_new_from_syspath(udev_device_get_udev(device.m_dev),
                                         udev_device_get_syspath(device.m_dev));
}

UsbDevice::~UsbDevice(void)
{
    if (m_dev)
    {
        udev_device_unref(m_dev);
    }
}
