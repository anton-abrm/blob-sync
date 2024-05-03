#pragma once

#include <string>
#include <format>

namespace BS::App {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
};

class Logger {
public:
    template<typename... Args>
    void debug(const std::string& message, Args&&... args) {
        if (m_min_level <= BS::App::LogLevel::Debug) {
            log(BS::App::LogLevel::Debug,
                std::vformat(message, std::make_format_args(args...)));
        }
    };

    template<typename... Args>
    void info(const std::string& message, Args&&... args) {
        if (m_min_level <= BS::App::LogLevel::Info) {
            log(BS::App::LogLevel::Info,
                std::vformat(message, std::make_format_args(args...)));
        }
    };

    template<typename... Args>
    void warning(const std::string& message, Args&&... args) {
        if (m_min_level <= BS::App::LogLevel::Warning) {
            log(BS::App::LogLevel::Warning,
                std::vformat(message, std::make_format_args(args...)));
        }
    };

    template<typename... Args>
    void error(const std::string& message, Args&&... args) {
        if (m_min_level <= BS::App::LogLevel::Error) {
            log(BS::App::LogLevel::Error,
                std::vformat(message, std::make_format_args(args...)));
        }
    };

    virtual void log(BS::App::LogLevel level, const std::string& message) = 0;

    void set_min_level (BS::App::LogLevel level) { m_min_level = level; };

    BS::App::LogLevel min_level() { return m_min_level; };

    virtual ~Logger();

private:
    BS::App::LogLevel m_min_level {LogLevel::Info};
};
}
