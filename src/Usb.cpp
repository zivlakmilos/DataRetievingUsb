#include "Usb.h"

Usb::Usb(void)
    : m_udev(nullptr)
{
    m_udev = udev_new();
}

Usb::~Usb(void)
{
    if (m_udev)
    {
        udev_unref(m_udev);
    }
}

std::vector<UsbDevice> Usb::getBlockDeviceList(void)
{
    std::vector<UsbDevice> result;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *deviceList, *devListEntry;
    struct udev_device *dev;

    enumerate = udev_enumerate_new(m_udev);
    if (!enumerate)
    {
        return result;
    }

    udev_enumerate_add_match_subsystem(enumerate, "block");
    udev_enumerate_scan_devices(enumerate);

    deviceList = udev_enumerate_get_list_entry(enumerate);
    if (!deviceList)
    {
        return result;
    }

    udev_list_entry_foreach(devListEntry, deviceList)
    {
        const char *path;

        path = udev_list_entry_get_name(devListEntry);
        dev = udev_device_new_from_syspath(m_udev, path);

        result.push_back(UsbDevice(dev));
    }

    udev_enumerate_unref(enumerate);

    return result;
}

bool Usb::isBlockDeviceExists(const std::string &syspath)
{
    struct udev_device *dev;

    dev = udev_device_new_from_syspath(m_udev, syspath.c_str());
    if (!dev)
    {
        return false;
    }

    return true;
}

UsbDevice Usb::getBlockDevice(const std::string& syspath)
{
    struct udev_device *dev;

    dev = udev_device_new_from_syspath(m_udev, syspath.c_str());

    return UsbDevice(dev);
}
