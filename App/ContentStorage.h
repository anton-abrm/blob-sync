#pragma once

#include <filesystem>
#include <map>
#include <vector>
#include <set>
#include <unordered_set>

#include "App/Logger.h"

namespace BS::App {

    class Repository;

    class ContentStorage final {

    public:
        explicit ContentStorage(BS::App::Logger & logger,
                                std::filesystem::path content_dir);

        void scan_content_files();
        void scan_cached_blobs();
        void scan_remote_blobs(const std::string& remote_url);
        void remove_duplicates_preserve_first();
        void remove_duplicates_preserve_last();

        [[nodiscard]] const std::map<std::vector<uint8_t>, std::unordered_set<std::filesystem::path>> & content_files() const { return m_content_files; };

        void download_blobs(const BS::App::Repository& repository, const std::string& remote_url);
        void upload_blobs(const BS::App::Repository& repository, const std::string& remote_url, bool prune);
        void restore_from(const BS::App::Repository& repository);

    private:
        const std::filesystem::path m_content_dir;

        std::set<std::vector<uint8_t>> m_cached_blobs;
        std::map<std::vector<uint8_t>, std::unordered_set<std::filesystem::path>> m_content_files;
        std::set<std::vector<uint8_t>> m_remote_blobs;

        [[nodiscard]] std::filesystem::path cache_dir() const;
        [[nodiscard]] std::filesystem::path blobs_dir() const;

        BS::App::Logger & m_logger;

    };
}

