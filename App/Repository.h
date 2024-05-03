#pragma once

#include <filesystem>
#include <set>
#include <unordered_set>
#include <map>
#include <vector>

#include "App/Logger.h"

namespace BS::App {

    class ContentStorage;

    class Repository final {
    public:
        explicit Repository(BS::App::Logger &logger,
                            std::filesystem::path repository_path);

        void scan_committed_blobs();
        void scan_workdir_files();

        [[nodiscard]] const std::set<std::vector<uint8_t>> & committed_blobs() const { return m_committed_blobs; };
        [[nodiscard]] const std::map<std::vector<uint8_t>, std::unordered_set<std::filesystem::path>> & workdir_files() const { return m_workdir_files; };

        void restore_from(const BS::App::ContentStorage& content_storage);

    private:
        std::filesystem::path m_repository_path;
        std::set<std::vector<uint8_t>> m_committed_blobs;
        std::map<std::vector<uint8_t>, std::unordered_set<std::filesystem::path>> m_workdir_files;

        BS::App::Logger &m_logger;
    };
}
