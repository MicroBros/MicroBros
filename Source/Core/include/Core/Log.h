#include <fmt/format.h>

namespace Core
{

/*! \brief Logger implementation for Firmware and Simulator
 *
 *  It contains a function to print pre-formatted information
 */
class Logger
{
public:
    //! Enum of the various LogLevel values possible
    enum class LogLevel
    {
        //! The highest priority, it will bypass printing the log level, useful for data
        None,
        //! Used for logging errors
        Error,
        //! Used for logging warnings
        Warn,
        //! Informal logging
        Info,
        //! Logging debug values
        Debug
    };

    //! Manually log some text at a set LogLevel with optional newline inserted
    static void Log(LogLevel level, std::string_view text, bool newline = true);
};

} // namespace Core

// Helper macros using fmt
#define LOG(text, ...)                                                                             \
    Core::Logger::Log(Core::Logger::LogLevel::None, fmt::format(text, ##__VA_ARGS__), false)
#define LOG_LEVEL(level, text, ...) Core::Logger::Log(level, fmt::format(text, ##__VA_ARGS__))
#define LOG_ERROR(format, ...) LOG_LEVEL(Core::Logger::LogLevel::Error, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) LOG_LEVEL(Core::Logger::LogLevel::Warn, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) LOG_LEVEL(Core::Logger::LogLevel::Info, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) LOG_LEVEL(Core::Logger::LogLevel::Debug, format, ##__VA_ARGS__)
