#pragma once

#include <vector>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>
#include <istream>

namespace BS {

    class Crypto {
    public:
        static std::vector<uint8_t> compute_sha_256(std::span<const uint8_t> input);
        static std::vector<uint8_t> compute_sha_256(std::istream &input, const std::size_t buffer_size);
    };
}
