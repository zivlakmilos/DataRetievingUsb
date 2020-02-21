#include <chrono>
#include <thread>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>

#include "Log.h"
#include "Settings.h"
#include "Usb.h"
#include "DataHandling.h"

volatile bool g_running;

void signalHandler(int sig)
{
    switch (sig)
    {
        case SIGHUP:
            break;
        case SIGTERM:
            g_running = false;
            break;
    }
}

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
    signal(SIGHUP, signalHandler); /* catch hangup signal */
    signal(SIGTERM, signalHandler); /* catch kill signal */

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
    for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
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

    g_running = true;

    std::chrono::steady_clock::time_point lastUploadTime = std::chrono::steady_clock::now();

    while (g_running)
    {
        auto settings = Settings().jsonSettings();
        std::string mountPoint = settings["Device"]["MountPoint"];
        std::string ignoreDevice1 = settings["Device"]["IgnoreDevice1"];
        std::string ignoreDevice2 = settings["Device"]["IgnoreDevice2"];

        std::vector<UsbDevice> deviceList = usb.getBlockDeviceList();

        DataHandling dh(mountPoint);

        for (auto device : deviceList)
        {
            if (device.getDevType() == "disk" || device.getDevNode().find(ignoreDevice1) != std::string::npos || device.getDevNode().find(ignoreDevice2) != std::string::npos)
            {
                continue;
            }

            bool success = false;

            for (int i = 0; i < 3; i++)
            {
                if (device.mount(mountPoint))
                {
                    success = true;
                    break;
                }
            }

            if (!success)
            {
                LOG_WARN("Failed to mount device: {0}", device.getDevNode());
                continue;
            }

            LOG_INFO("Device mounted: {0} -> {1}", device.getDevNode(), mountPoint);

            dh.fileCopy();

            while (!device.umount());
            LOG_INFO("Device unmounted: {0} -> {1}", device.getDevNode(), mountPoint);
        }

        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastUploadTime).count() >= 30)
        {
            dh.uploadLocalFiles();
            //lastUploadTime = currentTime;
            break;
        }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1000ms);
    }

    LOG_INFO("---------------------------------------------------------------");
    LOG_INFO("Service stop");
    LOG_INFO("---------------------------------------------------------------");

    return 0;
}
