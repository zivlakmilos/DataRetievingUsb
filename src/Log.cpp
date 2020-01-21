#include "Log.h"

#include <spdlog/sinks/basic_file_sink.h>

#include "Settings.h"

Log::Log(void)
{
    try
    {
        m_logger = spdlog::basic_logger_mt("DataRetreiveUsbRpi", "DataRetreiveUsbRpi.log");
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
