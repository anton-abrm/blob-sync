#pragma once

#include <string>
#include <vector>

namespace BS {
    class StringUtil final {
    public:
        [[nodiscard]] static std::string trim(const std::string &s);
        [[nodiscard]] static std::string quoted(const std::string &s);
        [[nodiscard]] static std::vector<std::string_view> split(std::string_view s, char delimiter);
    };
}

