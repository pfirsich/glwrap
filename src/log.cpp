#include "glw/log.hpp"

#include <cstdio>
#include <ctime>

#include <fmt/chrono.h>
#include <fmt/format.h>

namespace glw {

std::string_view toString(LogLevel level)
{
    switch (level) {
    case LogLevel::Debug:
        return "debug";
    case LogLevel::Info:
        return "info";
    case LogLevel::Warning:
        return "warning";
    case LogLevel::Error:
        return "error";
    case LogLevel::Critical:
        return "critical";
    default:
        return "unknown";
    }
}

std::vector<LoggingHandler> loggingHandlers;

void defaultLogger(LogLevel level, const char* filename, int line, const std::string& message)
{
    // Should this be Error?
    const auto t = std::time(nullptr);
    const auto toStderr = static_cast<int>(level) >= static_cast<int>(LogLevel::Warning);
    fmt::print(toStderr ? stderr : stdout, "[{:%Y-%m-%d %H:%M:%S}] [{}] [{}:{}] {}\n",
        *std::localtime(&t), level, filename, line, message);
}

}

auto fmt::formatter<glw::LogLevel>::format(
    glw::LogLevel level, format_context& ctx) const -> format_context::iterator
{
    return formatter<std::string_view>::format(toString(level), ctx);
}