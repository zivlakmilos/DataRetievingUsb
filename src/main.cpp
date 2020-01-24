#include <chrono>
#include <thread>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Log.h"
#include "Settings.h"
#include "Usb.h"
#include "DataHandling.h"

static void daemonize()
{
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }

    /* Open the log file */
    //openlog ("firstdaemon", LOG_PID, LOG_DAEMON);
}

int main(int argc, char *argv[])
{
    daemonize();

    LOG_INFO("---------------------------------------------------------------");
    LOG_INFO("Service start");
    LOG_INFO("---------------------------------------------------------------");

    Usb usb;

    DataHandling dh("data/local");
    dh.uploadLocalFiles();

    while (1)
    {
        auto settings = Settings().jsonSettings();
        std::string mountPoint = settings["Data"]["MountPoint"];

        std::vector<UsbDevice> deviceList = usb.getBlockDeviceList();

        DataHandling dh(mountPoint);

        for (auto device : deviceList)
        {
            if (device.getDevType() == "disk" || device.getDevNode().find("/dev/sda") != std::string::npos)
            {
                continue;
            }

            if (!device.mount(mountPoint))
            {
                LOG_WARN("Failed to mount device: {0}", device.getDevNode());
                continue;
            }

            LOG_WARN("Device mounted: {0} -> {1}", device.getDevNode(), mountPoint);

            dh.fileCopy();

            device.umount();
        }

        dh.uploadLocalFiles();

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(250ms);
    }

    LOG_INFO("---------------------------------------------------------------");
    LOG_INFO("Service stop");
    LOG_INFO("---------------------------------------------------------------");

    return 0;
}
