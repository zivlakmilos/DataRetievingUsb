#include "UsbDevice.h"

#include <sys/mount.h>

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

bool UsbDevice::mount(const std::string &target)
{
    if (!m_mountTarget.empty())
    {
        return false;
    }

    if (!::mount(getDevNode().c_str(), target.c_str(), "vfat", MS_NOATIME, NULL))
    {
        return false;
    }
    m_mountTarget = target;

    return true;
}

bool UsbDevice::umount(void)
{
    if (m_mountTarget.empty())
    {
        return true;
    }

    if (!::umount(m_mountTarget.c_str()))
    {
        return false;
    }

    return true;
}
