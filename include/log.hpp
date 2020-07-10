#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>

#include "fmt.hpp"

namespace glw {
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2, // It looks like the library was used wrong
    Error = 3, // Something went wrong, but we can keep going
    Critical = 4, // Something went wrong and we can't keep going
};

extern const std::unordered_map<LogLevel, std::string> logLevelNames;

using LoggingHandler = std::function<void(LogLevel, const char*, int, const std::string&)>;
extern std::vector<LoggingHandler> loggingHandlers;

void defaultLogger(LogLevel level, const char* filename, int line, const std::string& message);

template <typename... Args>
void log(LogLevel level, const char* filename, int line, const std::string& format, Args&&... args)
{
    if (loggingHandlers.empty())
        loggingHandlers.push_back(defaultLogger);

    const auto msg = fmt::format(format, std::forward<Args>(args)...);
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
struct fmt::formatter<glw::LogLevel> {
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(glw::LogLevel level, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}", glw::logLevelNames.at(level));
    }
};
