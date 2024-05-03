#pragma once

#include "App/Logger.h"

namespace BS::App {
    class ConsoleLogger final : virtual public BS::App::Logger{
    public:
        void log(BS::App::LogLevel level, const std::string &message) override;
    };
}
