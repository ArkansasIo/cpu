// logging.cpp - implementation of the levelled logger.
#include "common/Logging.h"

#include <ctime>
#include <iostream>

namespace ps3clone
{

    namespace
    {
        const char *LevelTag(LogLevel level)
        {
            switch (level)
            {
            case LogLevel::Trace:
                return "T";
            case LogLevel::Debug:
                return "D";
            case LogLevel::Info:
                return "I";
            case LogLevel::Warning:
                return "W";
            case LogLevel::Error:
                return "E";
            }
            return "?";
        }
    } // namespace

    Logger &Logger::Instance()
    {
        static Logger instance;
        return instance;
    }

    void Logger::Write(LogLevel level, const std::string &channel, const std::string &message)
    {
        if (static_cast<int>(level) < static_cast<int>(m_level))
        {
            return;
        }

        std::time_t now = std::time(nullptr);
        std::tm tm{};
#if defined(_WIN32)
        localtime_s(&tm, &now);
#else
        localtime_r(&now, &tm);
#endif

        char stamp[16];
        std::strftime(stamp, sizeof(stamp), "%H:%M:%S", &tm);

        std::printf("%s {%s} %s: %s\n", stamp, LevelTag(level), channel.c_str(), message.c_str());
        std::fflush(stdout);
    }

    void LogInfo(const std::string &channel, const std::string &msg)
    {
        Logger::Instance().Write(LogLevel::Info, channel, msg);
    }
    void LogWarn(const std::string &channel, const std::string &msg)
    {
        Logger::Instance().Write(LogLevel::Warning, channel, msg);
    }
    void LogError(const std::string &channel, const std::string &msg)
    {
        Logger::Instance().Write(LogLevel::Error, channel, msg);
    }

} // namespace ps3clone
