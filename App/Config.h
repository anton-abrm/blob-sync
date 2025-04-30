#pragma once

#include <string>
#include <optional>
#include <filesystem>
#include <unordered_map>

#include <App/StorageConfig.h>

namespace BS::App {
    class Config final {
    public:

        explicit Config(const std::filesystem::path &repository_path);

        [[nodiscard]] std::filesystem::path content_dir() const { return m_content_dir; }
        [[nodiscard]] const std::unordered_map<std::string, BS::App::StorageConfig> & storages() const { return m_storages; }

    private:
        std::filesystem::path m_content_dir;
        std::unordered_map<std::string, BS::App::StorageConfig> m_storages;
    };
}
