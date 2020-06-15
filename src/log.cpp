#include <cstdio>
#include <ctime>

#include <iostream>

#include "log.hpp"

namespace glw {
const std::unordered_map<LogLevel, std::string> logLevelNames {
    { LogLevel::Debug, "debug" },
    { LogLevel::Info, "info" },
    { LogLevel::Warning, "warning" },
    { LogLevel::Error, "error" },
    { LogLevel::Critical, "critical" },
};

std::vector<LoggingHandler> loggingHandlers;

void defaultLogger(LogLevel level, const char* filename, int line, const std::string& message)
{
    // Should this be Error?
    const auto t = std::time(nullptr);
    const auto toStderr = static_cast<int>(level) >= static_cast<int>(LogLevel::Warning);
    fmt::print(toStderr ? stderr : stdout, "[{%Y-%m-%d %H:%M:%S}] [{}] [{}:{}] {}", t, level,
        filename, line, message);
}
}
