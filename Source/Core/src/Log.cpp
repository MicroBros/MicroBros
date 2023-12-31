#ifdef FIRMWARE
#include <MicroBit.h>

extern MicroBit uBit;
#endif

#include "Core/Log.h"

namespace Core
{

std::string_view GetLogLevelString(const Logger::LogLevel level)
{
    switch (level)
    {
    case Logger::LogLevel::Error:
        return "ERROR";
    case Logger::LogLevel::Warn:
        return "WARN ";
    case Logger::LogLevel::Info:
        return "INFO ";
    case Logger::LogLevel::Debug:
        return "DEBUG";
    default:
        return "";
    }
}

void Logger::Log(Logger::LogLevel level, std::string_view text, bool newline)
{
#ifdef FIRMWARE

#if 0
    // Print the log level
    if (level != LogLevel::None)
    {
        auto level_str{GetLogLevelString(level)};
        for (char c : level_str)
        {
            uBit.serial.putc(c);
        }

        uBit.serial.putc(' ');
    }

    for (char c : text)
    {
        uBit.serial.putc(c);
    }

    if (newline)
        uBit.serial.putc('\n');
#endif

#else
    if (level != LogLevel::None)
        fmt::print("{} {}", GetLogLevelString(level), text);
    else
        fmt::print("{}", text);

    if (newline)
        fmt::print("\n");
#endif
}
}; // namespace Core
