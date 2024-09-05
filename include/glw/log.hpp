#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <fmt/base.h>

#include "glw/fmt.hpp"

namespace glw {
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2, // It looks like the library was used wrong
    Error = 3, // Something went wrong, but we can keep going
    Critical = 4, // Something went wrong and we can't keep going
};

std::string_view toString(LogLevel level);

using LoggingHandler = std::function<void(LogLevel, const char*, int, const std::string&)>;
extern std::vector<LoggingHandler> loggingHandlers;

void defaultLogger(LogLevel level, const char* filename, int line, const std::string& message);

template <typename... Args>
void log(LogLevel level, const char* filename, int line, std::string_view format, Args&&... args)
{
    if (loggingHandlers.empty())
        loggingHandlers.push_back(defaultLogger);

    const auto msg = fmt::format(fmt::runtime(format), std::forward<Args>(args)...);
    for (auto& handler : loggingHandlers)
        handler(level, filename, line, msg);
}

// http://stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick
#define LOG_LEVEL(level, ...) glw::log(level, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_LEVEL(glw::LogLevel::Debug, __VA_ARGS__)
#define LOG_INFO(...) LOG_LEVEL(glw::LogLevel::Info, __VA_ARGS__)
#define LOG_WARNING(...) LOG_LEVEL(glw::LogLevel::Warning, __VA_ARGS__)
#define LOG_ERROR(...) LOG_LEVEL(glw::LogLevel::Error, __VA_ARGS__)
#define LOG_CRITICAL(...) LOG_LEVEL(glw::LogLevel::Critical, __VA_ARGS__)
}

template <>
struct fmt::formatter<glw::LogLevel> : formatter<std::string_view> {
    auto format(glw::LogLevel level, format_context& ctx) const -> format_context::iterator;
};
