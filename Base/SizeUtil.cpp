#include "SizeUtil.h"

#include <array>
#include <format>

std::string BS::SizeUtil::format_size(const uint64_t size) {

    constexpr std::array suffixes {"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB"};

    auto size_double = static_cast<double>(size);
    size_t index {0};

    while (suffixes.size() - index > 1 && size_double >= 1024) {
        size_double /= 1024;
        ++index;
    }

    return std::vformat("{:.1f} {}", std::make_format_args(size_double, suffixes[index]));
}
