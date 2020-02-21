#include "DataHandling.h"

#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <cstdio>
#include <ctime>
#include <curl/curl.h>

#include "Settings.h"
#include "Log.h"

static size_t curlWriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

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
    std::string fileExtension = settings["Data"]["InputFileExtension"];
    std::string localFilePath = settings["Data"]["LocalFilePath"];

    //uint64_t currentTimestamp = timestamp();
    std::string currentTime = timestampString();

    for (const auto &entry : std::filesystem::directory_iterator(m_basePath))
    {
        std::string iFileName = entry.path().string();

        if (iFileName.find("." + fileExtension) == std::string::npos)
        {
            continue;
        }

        std::ifstream iFile(iFileName);
        if (!iFile.is_open())
        {
            LOG_ERROR("Cannot open file for read: {0}", iFileName);
            return false;
        }

        std::string line;
        getline(iFile, line);
        std::string header = line;

        /*
         * TODO: Header parseing
         */
        /*
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
        */

        std::string deviceId = "0";

        std::string oFileName = localFilePath + "/DB" + deviceId + " " + currentTime + ".txt";
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

        LOG_ERROR("File copied: {0} -> {1}", iFileName, oFileName);
    }

    return true;
}

void DataHandling::uploadLocalFiles(void)
{
    auto settings = Settings().jsonSettings();
    std::string localFilePath = settings["Data"]["LocalFilePath"];
    std::string uploadFilePath = settings["Data"]["UploadFilePath"];

    for (const auto &entry : std::filesystem::directory_iterator(localFilePath))
    {
        std::string fileName = entry.path().string();
        LOG_INFO("Try to upload file: {0}", fileName);
        if (uploadLocalFile(fileName))
        {
            LOG_INFO("File uploaded: {0}", fileName);

            std::ifstream iFile(fileName);
            std::ofstream oFile(uploadFilePath + "/" + entry.path().filename().string());

            if (!iFile.is_open() || !oFile.is_open())
            {
                LOG_WARN("Error while moving uploaded file: {0}", fileName);
            }

            std::string line;
            while (getline(iFile, line))
            {
                oFile << line;
            }
            iFile.close();
            oFile.flush();
            oFile.close();

            std::remove(fileName.c_str());
        }
        else
        {
            LOG_WARN("Error while uploading file: {0}", fileName);
        }
    }
}

bool DataHandling::uploadLocalFile(const std::string &fileName)
{
    auto settings = Settings().jsonSettings();
    std::string url = settings["InfluxDB"]["Url"];
    std::string database = settings["InfluxDB"]["Database"];
    std::string username = settings["InfluxDB"]["Username"];
    std::string password = settings["InfluxDB"]["Password"];

    url += "/write?u=" + username + "&p=" + password + "&db=" + database;

    std::ifstream iFile(fileName);

    if (!iFile.is_open())
    {
        LOG_WARN("Error while read file: {0}", fileName);
        return false;
    }

    auto curl = curl_easy_init();
    if (!curl)
    {
        LOG_WARN("Error while initializing curl");
        return false;
    }

    std::string line;
    /*
     * TODO: Read and ignore header
     */
    /*
    getline(iFile, line);
    std::string header = line;

    size_t pos = header.find(":");
    if (pos == std::string::npos)
    {
        LOG_WARN("Wrong header");
        return false;
    }
    if (header.substr(0, pos) != "DeviceId")
    {
        LOG_WARN("Wrong header");
        return false;
    }

    header.erase(0, pos + 1);
    std::string deviceId = header;
    */

    std::ostringstream oss;
    //oss << "DB" << deviceId << " line0=" << line << std::endl;

    while (getline(iFile, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::stringstream stringStream(line);
        std::vector<std::string> stringList;

        std::string segment;
        while (getline(stringStream, segment, ';'))
        {
            stringList.push_back(segment);
        }

        if (stringList.size() != 19)
        {
            continue;
        }

        std::string deviceId = stringList[0];
        std::string timestamp = stringList[1];

        oss << deviceId
            << " col1="  << stringList[2]
            << " col2="  << stringList[3]
            << " col3="  << stringList[4]
            << " col4="  << stringList[5]
            << " col5="  << stringList[6]
            << " col6="  << stringList[7]
            << " col7="  << stringList[8]
            << " col8="  << stringList[9]
            << " col9="  << stringList[10]
            << " col10=" << stringList[11]
            << " col11=" << stringList[12]
            << " col12=" << stringList[13]
            << " col13=" << stringList[14]
            << " col14=" << stringList[15]
            << " col16=" << stringList[16]
            << " col17=" << stringList[17]
            << " col18=" << stringList[18]
            << " " << timestamp
            << std::endl;

        //oss << "DB" << deviceId << " line" << i << "=" << line << std::endl;
    }

    std::string data = oss.str();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    auto res = curl_easy_perform(curl);

    bool success = false;
    if (res == 0 && response.empty())
    {
        success = true;
    }
    else
    {
        LOG_WARN("Error while communicating with server: {0}", response);
    }

    curl_easy_cleanup(curl);
    curl = NULL;

    return success;
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
    oss << std::put_time(&tm, "%y-%M-%m %H-%M-%S");

    return oss.str();
}

static size_t curlWriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
