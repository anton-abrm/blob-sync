#pragma once

#include <string>
#include <string_view>
#include <istream>
#include <ostream>
#include <filesystem>
#include <map>
#include <span>

#include "App/ContentFileInfo.h"

namespace BS::App {
    class ContentFileCache final {
    public:
        static std::map<std::vector<uint8_t>, std::vector<uint8_t>> load(std::istream &input);
        static void save(std::ostream &output, const std::map<std::vector<uint8_t>, std::vector<uint8_t>> &file_infos);
        static std::vector<uint8_t> compute_fingerprint(const BS::App::ContentFileInfo &fi);
    };
}
