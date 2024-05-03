#pragma once

#include <string>
#include <vector>

namespace BS {
    class SystemCommand final {
    public:
        [[nodiscard]]
        static std::vector<std::string> exec_lines(const std::string &command);

        static void exec(const std::string &command);
    };
}