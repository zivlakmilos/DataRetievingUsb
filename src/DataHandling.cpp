#include "DataHandling.h"

#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <cstdio>
#include <ctime>

#include "Settings.h"
#include "Log.h"

DataHandling::DataHandling(const std::string &basePath)
    : m_basePath(basePath)
{
}

DataHandling::~DataHandling(void)
{
}

bool DataHandling::fileExists(void)
{
    auto settings = Settings().jsonSettings();
    std::string fileName = settings["Data"]["InputFile"];

    std::ifstream iFile(m_basePath + "/" + fileName);

    return iFile.is_open();
}

bool DataHandling::fileCopy(void)
{
    auto settings = Settings().jsonSettings();
    std::string fileName = settings["Data"]["InputFile"];
    std::string localFilePath = settings["Data"]["LocalFilePath"];

    //uint64_t currentTimestamp = timestamp();
    std::string currentTime = timestampString();

    std::string iFileName = m_basePath + "/" + fileName;
    std::ifstream iFile(iFileName);
    if (!iFile.is_open())
    {
        LOG_ERROR("Cannot open file for read: {0}", iFileName);
        return false;
    }

    std::string line;
    getline(iFile, line);
    std::string header = line;

    size_t pos = header.find(":");
    if (pos == std::string::npos)
    {
        LOG_ERROR("Wrong header");
        return false;
    }
    if (header.substr(0, pos) != "DeviceId")
    {
        LOG_ERROR("Wrong header");
        return false;
    }

    header.erase(0, pos + 1);
    std::string deviceId = header;

    std::string oFileName = localFilePath + "/DB" + deviceId + currentTime + ".txt";
    std::ofstream oFile(oFileName);

    if (!oFile.is_open())
    {
        LOG_ERROR("Cannot open file for write: {0}", oFileName);
        return false;
    }

    oFile << line << std::endl;

    while (getline(iFile, line))
    {
        oFile << line << std::endl;
    }

    oFile.flush();
    oFile.close();
    iFile.close();

    std::remove(iFileName.c_str());

    return true;
}

bool DataHandling::uploadLocalFiles(void)
{
    auto settings = Settings().jsonSettings();
    std::string localFilePath = settings["Data"]["LocalFilePath"];
    std::string UploadFilePath = settings["Data"]["UploadFilePath"];

    for (const auto &entry : std::filesystem::directory_iterator(localFilePath))
    {
        std::ostringstream oss;
        oss << entry.path();
        std::string fileName = oss.str();
        LOG_INFO("Try to upload file: {0}", fileName);
        if (uploadLocalFile(fileName))
        {
            LOG_INFO("File uploaded: {0}", fileName);
        }
        else
        {
            LOG_WARN("Error while uploading file: {0}", fileName);
        }
    }

    return true;
}

bool DataHandling::uploadLocalFile(const std::string &fileName)
{
    std::ifstream iFile(fileName);

    if (!iFile.is_open())
    {
        return false;
    }

    std::string line;
    while (getline(iFile, line))
    {
        // TODO: Upload line
    }

    return true;
}

uint64_t DataHandling::timestamp(void)
{
    std::chrono::high_resolution_clock clock;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(clock.now().time_since_epoch()).count();
}

std::string DataHandling::timestampString(void)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");

    return oss.str();
}
