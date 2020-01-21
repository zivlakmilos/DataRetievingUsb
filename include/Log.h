#ifndef _LOG_H_
#define _LOG_H_

#include <memory>
#include <spdlog/spdlog.h>

class Log
{
public:
    Log(const Log &) = delete;
    void operator=(const Log &) = delete;

    static Log &log(void);

    inline std::shared_ptr<spdlog::logger> coreLogger() { return m_logger; }

private:
    Log(void);

    std::shared_ptr<spdlog::logger> m_logger;
};

#define LOG_ERROR(...) Log::log().coreLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) Log::log().coreLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...) Log::log().coreLogger()->info(__VA_ARGS__)

#endif // _LOG_H_
