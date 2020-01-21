#include "Log.h"

#include <spdlog/sinks/basic_file_sink.h>

#include "Settings.h"

Log::Log(void)
{
    try
    {
        auto settings = Settings().jsonSettings()["Logging"];
        std::string logName = settings["Name"];
        std::string logFile = settings["File"];
        //std::string logLevel = settings["Level"];

        m_logger = spdlog::basic_logger_mt(logName, logFile);
        m_logger->set_level(spdlog::level::trace);
    }
    catch (...)
    {
    }
}

Log &Log::log(void)
{
    static Log log;
    return log;
}
