#pragma once

#include <vector>
#include <chrono>
#include <span>
#include <filesystem>

namespace BS::App {
    class ContentFileInfo final {
    public:
        [[nodiscard]] std::chrono::nanoseconds modify_time() const;
        [[nodiscard]] std::size_t file_size() const;
        [[nodiscard]] std::filesystem::path relative_path() const;

        void set_relative_path(const std::filesystem::path &path);
        void set_modify_time(std::chrono::nanoseconds value);
        void set_file_size(std::size_t value);

    private:
        std::filesystem::path m_relative_path;
        std::chrono::nanoseconds m_modify_time {};
        std::size_t m_file_size {};
    };
}
