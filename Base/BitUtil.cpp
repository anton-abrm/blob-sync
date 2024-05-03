#include "BitUtil.h"

#include <stdexcept>
#include <bit>
#include <algorithm>

std::vector<uint8_t> BS::BitUtil::to_bytes_be(uint64_t value) {

    std::vector<uint8_t> result(sizeof(value));

    if constexpr (std::endian::native == std::endian::big) {
        *reinterpret_cast<uint64_t *>(result.data()) = value;
        return result;
    }

    if constexpr (std::endian::native == std::endian::little) {
        *reinterpret_cast<uint64_t *>(result.data()) = value;
        std::reverse(result.begin(), result.end());
        return result;
    }

    for (std::size_t i = result.size(); i > 0; ) {
        --i;
        result[i] = static_cast<uint8_t>(value);
        value >>= 8;
    }

    return result;
}

uint64_t BS::BitUtil::to_uint64_be(std::span<const uint8_t> bytes) {

    if (bytes.size() != sizeof(uint64_t))
        throw std::invalid_argument("Invalid size of span.");

    if constexpr (std::endian::native == std::endian::big) {
        return *reinterpret_cast<const uint64_t *>(bytes.data());
    }

    if constexpr (std::endian::native == std::endian::little) {
        std::vector<uint8_t> v(bytes.rbegin(), bytes.rend());
        return *reinterpret_cast<const uint64_t *>(v.data());
    }

    uint64_t result = 0;

    for (const uint8_t b : bytes) {
        result <<= 8;
        result |= b;
    }

    return result;
}
