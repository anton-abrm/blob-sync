#pragma once

#include <string>
#include <cstdint>

namespace BS {
    class SizeUtil final {
    public:
        static std::string format_size(uint64_t size);
    };
}
