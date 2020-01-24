#include <iostream>

#include "Log.h"
#include "Settings.h"
#include "Usb.h"
#include "DataHandling.h"

#include "influxdb/InfluxDB.h"
#include "influxdb/InfluxDBFactory.h"
#include "influxdb/Point.h"

int main(int argc, char *argv[])
{
    LOG_INFO("Start");

    DataHandling dh("data/input");

    if (dh.fileCopy())
    {
        std::cout << "Copy success" << std::endl;
    }
    else
    {
        std::cout << "Copy fail" << std::endl;
    }

    if (dh.uploadLocalFiles())
    {
        std::cout << "Upload success" << std::endl;
    }
    else
    {
        std::cout << "Upload fail" << std::endl;
    }

    return 0;

    LOG_INFO("Test");

    try
    {
        auto influxdb = influxdb::InfluxDBFactory::Get("http://us-west-2-1.aws.cloud2.influxdata.com/?u=vvv10265@zzrgg.com&p=vvv10265@zzrgg.com");
        influxdb->write(influxdb::Point{"test"}
            .addField("value", 10)
            .addTag("host", "localhost")
        );
    } catch (...)
    {
        std::cout << "Eerror";
    }

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
