#pragma once

#include <string>
#include <filesystem>
#include <istream>

namespace BS::App {
    class Config final {
    public:
        [[nodiscard]] std::filesystem::path content_dir() const;
        [[nodiscard]] std::string storage_url() const;

        void set_content_dir(const std::filesystem::path &value);
        void set_storage_url(const std::string &value);

        static BS::App::Config load(const std::filesystem::path &repository_path);

    private:
        std::filesystem::path m_content_dir;
        std::string m_storage_url;
    };
}
