#pragma once

#include <chrono>
#include <vector>

namespace BS::App {
    class ContentFileCacheEntry final {

    public:
        [[nodiscard]] std::chrono::system_clock::time_point last_hit_time_minutes() const { return m_last_hit_time_minutes; }
        [[nodiscard]] const std::vector<uint8_t> & file_hash() const { return m_file_hash; }

        void set_last_hit_time_minutes(const std::chrono::system_clock::time_point last_hit_time_minutes) { m_last_hit_time_minutes = last_hit_time_minutes; }
        void set_file_hash(std::vector<uint8_t> file_hash) { m_file_hash = std::move(file_hash); }

        bool operator==(const ContentFileCacheEntry& rhs) const = default;

    private:
        std::vector<uint8_t> m_file_hash {};
        std::chrono::system_clock::time_point m_last_hit_time_minutes {};
    };
}
