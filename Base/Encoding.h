#pragma once

#include <string>
#include <vector>
#include <span>
#include <optional>
#include <cstdint>

namespace BS {

    class Encoding {
    public:
        static std::string encode_base64_url_no_padding(std::span<const uint8_t> bytes);
        static std::optional<std::vector<uint8_t>> decode_base64_any(std::string_view s);
        static std::string encode_hex_lower(std::span<const uint8_t> bytes);
        static std::string encode_hex_upper(std::span<const uint8_t> bytes);
        static std::optional<std::vector<uint8_t>> decode_hex_any(std::string_view s);
    };
}
