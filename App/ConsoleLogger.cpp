#include "ConsoleLogger.h"

#include <iostream>

void BS::App::ConsoleLogger::log(const BS::App::LogLevel level, const std::string &message) const {

    switch (level) {

        case LogLevel::Debug:
            std::cout << "[DEBUG] " << message << std::endl;
            break;

        case LogLevel::Info:
            std::cout << message << std::endl;
            break;

        case LogLevel::Warning:
            std::cout << "[WARNING] " << message << std::endl;
            break;

        case LogLevel::Error:
            std::cerr << "[ERROR] " << message << std::endl;
            break;
    }
}
