#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace BS {
    class GitConfig final {

    public:

        void merge(const std::filesystem::path& path);

        std::optional<bool> get_bool(const std::string& section, const std::string& key, const std::string& sub_section = std::string()) const;
        bool get_required_bool(const std::string& section, const std::string& key, const std::string& sub_section = std::string()) const;
        std::optional<std::string> get_string(const std::string& section, const std::string& key, const std::string& sub_section = std::string()) const;
        std::string get_required_string(const std::string& section, const std::string& key, const std::string& sub_section = std::string()) const;
        std::unordered_set<std::string> get_sub_sections(const std::string& section) const;

    private:
        std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::string>>> m_map;
    };
}
