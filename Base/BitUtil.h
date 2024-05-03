#pragma once

#include <vector>
#include <cstdint>
#include <span>

namespace BS {
    class BitUtil final {
    public:
        static std::vector<uint8_t> to_bytes_be(uint64_t value);
        static uint64_t to_uint64_be(std::span<const uint8_t> bytes);
    };
}