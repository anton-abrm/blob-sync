#include "StringUtil.h"

#include <sstream>
#include <iomanip>

std::string BS::StringUtil::trim(const std::string &s) {

    const auto start = s.find_first_not_of("\x20\x09");
    const auto end = s.find_last_not_of("\x20\x09");

    if (start == end)
        return s;

    return s.substr(start, end - start + 1);
}

std::string BS::StringUtil::quoted(const std::string &s) {
    std::stringstream ss;
    ss << std::quoted(s);
    return ss.str();
}

std::vector<std::string_view> BS::StringUtil::StringUtil::split(std::string_view s, const char delimiter) {

    size_t start = 0;

    std::vector<std::string_view> v;

    while (true)
    {
        size_t end = s.find(delimiter, start);

        if (end == std::string_view::npos)
            break;

        v.emplace_back(s.substr(start, end - start));

        start = end + 1;
    }

    v.emplace_back(s.substr(start, s.size() - start));

    return v;
}
