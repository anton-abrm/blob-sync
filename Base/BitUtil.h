#pragma once

#include <vector>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <bit>

namespace BS {
    class BitUtil final {
    public:
        template<std::integral T>
        static std::vector<uint8_t> to_bytes_be(T value) {

            std::vector<uint8_t> result(sizeof(value));

            if constexpr (std::endian::native == std::endian::little) {
                value = std::byteswap(value);
            }

            *reinterpret_cast<T *>(result.data()) = value;

            return result;
        }

        template<std::integral T>
        static T from_bytes_be(std::span<const uint8_t> bytes) {

            if (bytes.size() != sizeof(T))
                throw std::invalid_argument("Invalid size of span.");

            T value = *reinterpret_cast<const T *>(bytes.data());

            if constexpr (std::endian::native == std::endian::little) {
                value = std::byteswap(value);
            }

            return value;
        }
    };
}
