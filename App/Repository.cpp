#include "Repository.h"

#include <unordered_map>
#include <iostream>
#include <fstream>

#include "App/ContentStorage.h"

#include "Base/SystemCommand.h"
#include "Base/Encoding.h"
#include "Base/IOUtil.h"
#include "Base/StringUtil.h"
#include "Git/Git.h"

BS::App::Repository::Repository(BS::App::Logger &logger,
                                std::filesystem::path repository_path)
    : m_logger(logger)
    , m_repository_path(std::move(repository_path)) {
}

static std::vector<uint8_t> read_hash(const std::filesystem::path &path) {

    std::ifstream input(path, std::ios::binary);

    if (!input)
        throw std::runtime_error("Unable to open hash file.");

    std::string buffer(96, 0);

    if (!input.read(&buffer[0], static_cast<std::streamsize>(buffer.size())) && !input.eof())
        throw std::runtime_error("Unable to read hash file.");

    if (input.gcount() != 64)
        return {};

    return BS::Encoding::decode_hex_any({ &buffer[0], 64 }).value_or(std::vector<uint8_t>());
}

void BS::App::Repository::scan_committed_blobs() {

    m_committed_blobs.clear();

    BS::Git::instance().get_committed_blobs(m_repository_path, [&](auto content) {
        m_committed_blobs.insert(BS::Encoding::decode_hex_any(content).value());
    });

    m_logger.info("Committed blobs: {}", m_committed_blobs.size());
}

void BS::App::Repository::scan_workdir_files() {

    m_workdir_files.clear();

    for (auto const& fs_entry : std::filesystem::directory_iterator(m_repository_path)) {

        if (fs_entry.is_directory()) {

            if (fs_entry.path().filename() == ".git")
                continue;

            for (const auto& fs_child_entry : std::filesystem::recursive_directory_iterator(fs_entry.path())) {
                if (fs_child_entry.is_regular_file()) {
                    const auto relative_path = fs_child_entry.path().lexically_relative(m_repository_path);
                    const auto hash = read_hash(fs_child_entry.path());
                    m_workdir_files[hash].insert(relative_path);
                }
            }

            continue;
        }

        if (fs_entry.is_regular_file()) {
            const auto relative_path = fs_entry.path().lexically_relative(m_repository_path);
            const auto hash = read_hash(fs_entry.path());
            m_workdir_files[hash].insert(relative_path);
        }
    }

    m_logger.info("Unique workdir files: {}", m_workdir_files.size());
}

void BS::App::Repository::restore_from(const BS::App::ContentStorage& content_storage) {

    std::unordered_map<std::filesystem::path, std::vector<uint8_t>> content_files;

    for (const auto& [hash, relative_paths]: content_storage.content_files()) {
        for (const auto& relative_path: relative_paths) {
            content_files[relative_path] = hash;
        }
    }

    for (auto wf_it = m_workdir_files.begin(); wf_it != m_workdir_files.end();) {

        const auto& wf_hash = wf_it->first;
        auto& wf_relative_paths = wf_it->second;

        for (auto wfrp_it = wf_relative_paths.begin(); wfrp_it != wf_relative_paths.end();)
        {
            const auto& wf_relative_path = *wfrp_it;

            const auto cf_it = content_files.find(wf_relative_path);

            if (cf_it != content_files.end()) {

                if (wf_hash == cf_it->second) {
                    content_files.erase(cf_it);
                }

                ++wfrp_it;
            }
            else {
                m_logger.debug("Deleting {}", BS::StringUtil::quoted(wf_relative_path));
                std::filesystem::remove(m_repository_path / wf_relative_path);
                wfrp_it = wf_relative_paths.erase(wfrp_it);
            }
        }

        if (wf_relative_paths.empty()) {
            wf_it = m_workdir_files.erase(wf_it);
        }
        else {
            ++wf_it;
        }
    }

    for (const auto& [cf_relative_path, cf_hash] : content_files) {

        m_logger.debug("Restoring {}", BS::StringUtil::quoted(cf_relative_path));

        const auto path = m_repository_path / cf_relative_path;

        std::filesystem::create_directories(path.parent_path());

        std::ofstream out (path, std::ios::binary);

        out << BS::Encoding::encode_hex_upper(cf_hash);

        if (out.fail())
            throw std::runtime_error("Unable to write hash file.");
    }

    for (auto const& child_entry : std::filesystem::directory_iterator(m_repository_path)) {
        if (child_entry.is_directory() && child_entry.path().filename() != ".git") {
            BS::IOUtil::remove_empty_directories_recursively(child_entry);
        }
    }
}
