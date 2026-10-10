// logging.h - minimal levelled logger.
#pragma once

#include <cstdio>
#include <string>

namespace ps3clone
{

    enum class LogLevel
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error
    };

    class Logger
    {
    public:
        static Logger &Instance();

        void SetLevel(LogLevel level) { m_level = level; }
        LogLevel GetLevel() const { return m_level; }

        // Variadic-free core: callers format with std::string.
        void Write(LogLevel level, const std::string &channel, const std::string &message);

    private:
        Logger() = default;
        LogLevel m_level = LogLevel::Info;
    };

    // Convenience channels used across the clone.
    void LogInfo(const std::string &channel, const std::string &msg);
    void LogWarn(const std::string &channel, const std::string &msg);
    void LogError(const std::string &channel, const std::string &msg);

} // namespace ps3clone
