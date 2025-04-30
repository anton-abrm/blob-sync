#pragma once

#include <string>
#include <istream>
#include <ostream>
#include <map>

#include "App/ContentFileInfo.h"
#include "App/ContentFileCacheEntry.h"

namespace BS::App {
    class ContentFileCache final {
    public:
        static std::map<std::vector<uint8_t>, BS::App::ContentFileCacheEntry> load(std::istream &input);
        static void save(std::ostream &output, const std::map<std::vector<uint8_t>, BS::App::ContentFileCacheEntry> &file_infos);
        static std::vector<uint8_t> compute_fingerprint(const BS::App::ContentFileInfo &fi);
    };
}
