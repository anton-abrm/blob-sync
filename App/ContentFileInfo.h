#pragma once

#include <vector>
#include <chrono>
#include <span>
#include <filesystem>

namespace BS::App {
    class ContentFileInfo final {
    public:
        [[nodiscard]] std::filesystem::file_time_type modify_time() const;
        [[nodiscard]] std::size_t file_size() const;
        [[nodiscard]] std::filesystem::path relative_path() const;

        void set_relative_path(const std::filesystem::path &path);
        void set_modify_time(std::filesystem::file_time_type value);
        void set_file_size(std::size_t value);

    private:
        std::filesystem::path m_relative_path {};
        std::filesystem::file_time_type m_modify_time {};
        std::size_t m_file_size {};
    };
}
