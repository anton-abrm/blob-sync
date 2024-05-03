#include "ContentStorage.h"

#include <atomic>
#include <mutex>
#include <fstream>
#include <iostream>
#include <format>
#include <utility>
#include <algorithm>

#include "App/ContentFileCache.h"
#include "App/Repository.h"

#include "Base/Crypto.h"
#include "Base/Encoding.h"
#include "Base/SystemCommand.h"
#include "Base/IOUtil.h"
#include "Base/StringUtil.h"

void BS::App::ContentStorage::download_blobs(const BS::App::Repository& repository) {

    auto blobs_to_download = repository.committed_blobs();

    for (const auto &[hash, relative_paths] : m_content_files) {
        blobs_to_download.erase(hash);
    }

    for (const auto &hash : m_cached_blobs) {
        blobs_to_download.erase(hash);
    }

    m_logger.info("Blobs to download: {}", blobs_to_download.size());

    if (blobs_to_download.empty())
        return;

    std::filesystem::create_directories(blobs_dir());

    const auto download_list_path = cache_dir() / "down-list";

    {
        std::ofstream output(download_list_path);

        for (const auto &hash: blobs_to_download) {

            const auto hash_hex = BS::Encoding::encode_hex_upper(hash);

            output.write(reinterpret_cast<const char *>(hash_hex.data()), static_cast<std::streamsize>(hash_hex.size()));
            output.put('\n');

            if (output.fail())
                throw std::runtime_error("Unable to write hash list.");
        }
    }

    const auto cmd = std::format(
            "/usr/bin/rsync -ri --ignore-existing --include-from={} --exclude=* {} {}",
                StringUtil::quoted(download_list_path.string()),
                StringUtil::quoted(m_remote_url + "/"),
                StringUtil::quoted(blobs_dir().string() + "/"));

    m_logger.debug("{}", cmd);

    BS::SystemCommand::exec(cmd);

    scan_cached_blobs();
}

void BS::App::ContentStorage::upload_blobs(const BS::App::Repository& repository, bool prune) {

    auto blobs_to_upload = repository.committed_blobs();

    for (const auto &hash : m_remote_blobs) {
        blobs_to_upload.erase(hash);
    }

    m_logger.info("Blobs to upload: {}", blobs_to_upload.size());
    m_logger.info("Prune {}", (prune ? "On" : "Off"));

    if (blobs_to_upload.empty() && !prune)
        return;

    const auto& blobs_to_cache = prune
            ? repository.committed_blobs()
            : blobs_to_upload;

    std::filesystem::create_directories(blobs_dir());

    for (const auto &hash : blobs_to_cache) {

        if (m_cached_blobs.contains(hash)) {
            continue;
        }

        const auto blob_filename = BS::Encoding::encode_hex_upper(hash);

        const auto it = m_content_files.find(hash);

        if (it == m_content_files.end()) {
            m_logger.warning("Blob was not found: {}", blob_filename);
            continue;
        }

        const auto relative_path = *it->second.cbegin();

        const auto from_path = m_content_dir / relative_path;
        const auto to_path = blobs_dir() / blob_filename;

        m_logger.debug("Moving to cache {}", BS::StringUtil::quoted(relative_path));

        std::filesystem::rename(from_path, to_path);

        m_cached_blobs.insert(hash);

        it->second.erase(relative_path);

        if (it->second.empty()) {
            m_content_files.erase(hash);
        }
    }

    const auto upload_list_path = cache_dir() / "up-list";

    {
        std::ofstream output(upload_list_path);

        for (const auto &hash: blobs_to_cache) {

            const auto hash_hex = BS::Encoding::encode_hex_upper(hash);

            output.write(reinterpret_cast<const char *>(hash_hex.data()), static_cast<std::streamsize>(hash_hex.size()));
            output.put('\n');

            if (output.fail())
                throw std::runtime_error("Unable to write hash list.");
        }
    }

    const auto cmd = std::format(
            "/usr/bin/rsync -ri {} --ignore-existing --include-from={} --exclude=* {} {}",
            prune ? "--delete-excluded" : "",
            StringUtil::quoted(upload_list_path.string()),
            StringUtil::quoted(blobs_dir().string() + "/"),
            StringUtil::quoted(m_remote_url + "/"));

    m_logger.debug("{}", cmd);

    BS::SystemCommand::exec(cmd);

    for (const auto & hash: blobs_to_upload) {
        m_remote_blobs.insert(hash);
    }
}

BS::App::ContentStorage::ContentStorage(BS::App::Logger &logger,
                                        std::filesystem::path content_dir,
                                        std::string remote_url)
    : m_logger(logger)
    , m_content_dir(std::move(content_dir))
    , m_remote_url(std::move(remote_url)) {
}

void BS::App::ContentStorage::scan_cached_blobs() {

    m_cached_blobs.clear();

    for (auto const& fs_entry : std::filesystem::directory_iterator(blobs_dir())) {
        if (fs_entry.is_regular_file()) {
            m_cached_blobs.insert(
                    Encoding::decode_hex_any(fs_entry.path().filename().string()).value());
        }
    }

    m_logger.info("Cached blobs: {}", m_cached_blobs.size());
}

void BS::App::ContentStorage::scan_content_files() {

    m_content_files.clear();

    std::vector<std::filesystem::directory_entry> content_dir_entries;

    for (auto const& fs_entry : std::filesystem::directory_iterator(m_content_dir)) {

        if (fs_entry.is_directory()) {

            if (fs_entry.path().filename() == ".cache")
                continue;

            for (auto const& fs_child_entry : std::filesystem::recursive_directory_iterator(fs_entry.path())) {

                if (fs_child_entry.is_regular_file()) {
                    content_dir_entries.push_back(fs_child_entry);
                }
            }

            continue;
        }

        if (fs_entry.is_regular_file()) {
            content_dir_entries.push_back(fs_entry);
        }
    }

    const std::filesystem::path files_cache_path = cache_dir() / "file-list";

    std::mutex mutex;
    std::atomic_bool scan_error {false};
    std::map<std::vector<uint8_t>, std::vector<uint8_t>> content_cache;
    std::map<std::vector<uint8_t>, std::vector<uint8_t>> new_content_cache;

    try {
        std::ifstream files_cache_stream(files_cache_path, std::ios::binary);
        content_cache = BS::App::ContentFileCache::load(files_cache_stream);
    }
    catch(const std::runtime_error &) {
        m_logger.warning("{}", "The cache file is not available.");
    }

    std::for_each(content_dir_entries.begin(), content_dir_entries.end(), [&](const std::filesystem::directory_entry &dir_entry) {

        try {

            if (scan_error)
                return;

            const auto relative_path = dir_entry.path().lexically_relative(m_content_dir);
            const auto file_size = static_cast<std::size_t>(dir_entry.file_size());
            const auto modify_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::file_clock::to_sys(dir_entry.last_write_time()).time_since_epoch());


            BS::App::ContentFileInfo file_info;

            file_info.set_relative_path(relative_path);
            file_info.set_file_size(file_size);
            file_info.set_modify_time(modify_time);

            const auto fingerprint = BS::App::ContentFileCache::compute_fingerprint(file_info);

            std::vector<uint8_t> file_hash;

            {
                std::lock_guard<std::mutex> guard(mutex);

                const auto it = content_cache.find(fingerprint);
                if (it != content_cache.end()) {
                    file_hash = it->second;
                }
            }

            if (file_hash.empty()) {

                {
                    std::lock_guard<std::mutex> guard(mutex);
                    m_logger.info("Hashing {}", BS::StringUtil::quoted(relative_path.string()));
                }

                std::ifstream input(dir_entry.path(), std::ios::binary);

                file_hash = BS::Crypto::compute_sha_256(input, 1024 * 1024);
            }

            {
                std::lock_guard<std::mutex> guard(mutex);

                m_content_files[file_hash].insert(relative_path);

                new_content_cache[fingerprint] = file_hash;
            }
        }
        catch (const std::runtime_error &ex) {

            scan_error = true;

            {
                std::lock_guard<std::mutex> guard(mutex);
                std::cerr << ex.what() << std::endl;
            }
        }
    });

    if (scan_error)
        throw std::runtime_error("Unable to scan files.");

    std::filesystem::create_directories(cache_dir());

    {
        std::ofstream files_cache_stream(files_cache_path);
        BS::App::ContentFileCache::save(files_cache_stream, new_content_cache);
    }

    m_logger.info("Unique content files: {}", m_content_files.size());

    const auto duplicates = std::count_if(m_content_files.begin(), m_content_files.end(), [](const auto &kvp) {
        return kvp.second.size() >= 2;
    });

    m_logger.info("Duplicate content files: {}", duplicates);

    if (duplicates > 0) {

        auto wd_it = m_content_files.begin();

        for (; wd_it != m_content_files.end(); ++wd_it) {
            const auto &relative_paths = wd_it->second;
            if (relative_paths.size() >= 2) {
                m_logger.debug("---------");
                break;
            }
        }

        for (; wd_it != m_content_files.end(); ++wd_it) {

            const auto &hash = wd_it->first;
            const auto &relative_paths = wd_it->second;

            if (relative_paths.size() >= 2) {
                for (const auto &relative_path: relative_paths) {
                    m_logger.debug("Duplicate {} {}",
                                   BS::Encoding::encode_hex_upper({hash.begin(), hash.begin() + 4}),
                                   BS::StringUtil::quoted(relative_path));
                }
                m_logger.debug("---------");
            }
        }
    }
}

void BS::App::ContentStorage::scan_remote_blobs() {

    m_remote_blobs.clear();

    const auto cmd = std::format("/usr/bin/rsync --list-only {}",
        StringUtil::quoted(m_remote_url + "/"));

    m_logger.debug("{}", cmd);

    const auto lines = BS::SystemCommand::exec_lines(cmd);

    for (const auto& line: lines) {

        if (line.size() < 64)
            continue;

        const auto hash = BS::Encoding::decode_hex_any({line.end() - 64, line.end()});

        if (!hash)
            continue;

        m_remote_blobs.insert(hash.value());
    }

    m_logger.info("Remote blobs: {}", m_remote_blobs.size());
}

std::filesystem::path BS::App::ContentStorage::cache_dir() const {
    return m_content_dir / ".cache";
}

std::filesystem::path BS::App::ContentStorage::blobs_dir() const {
    return cache_dir() / "blobs";
}

void BS::App::ContentStorage::restore_from(const BS::App::Repository& repository) {

    // Cleaning cache
    for (auto it = m_cached_blobs.begin(); it != m_cached_blobs.end(); ) {

        const auto& cached_blob = *it;

        if (repository.committed_blobs().contains(cached_blob) ||
            repository.workdir_files().contains(cached_blob)) {
            ++it;
            continue;
        }

        const auto filename = BS::Encoding::encode_hex_upper(cached_blob);
        const auto path = blobs_dir() / filename;

        m_logger.debug("Deleting from cache {}", BS::StringUtil::quoted(filename));

        std::filesystem::remove(path);
        it = m_cached_blobs.erase(it);
    }

    // Moving to cache
    for (auto it = m_content_files.begin(); it != m_content_files.end(); ) {

        auto& [hash, relative_paths] = *it;

        auto rp_it = relative_paths.begin();

        const auto wd_it = repository.workdir_files().find(hash);

        bool move_to_cache = false;

        if (wd_it != repository.workdir_files().end()) {

            const auto& [wd_hash, wd_relative_paths] = *wd_it;

            if (relative_paths == wd_relative_paths) {
                ++it;
                continue;
            }

            move_to_cache = true;
        }
        else if (repository.committed_blobs().contains(hash)) {
            move_to_cache = true;
        }

        if (move_to_cache && !m_cached_blobs.contains(hash)) {

            const auto filename = BS::Encoding::encode_hex_upper(hash);
            const auto to_path = blobs_dir() / filename;
            const auto from_path = m_content_dir / *rp_it;

            m_logger.debug("Moving to cache {}", BS::StringUtil::quoted(rp_it->string()));
            std::filesystem::rename(from_path, to_path);
            m_cached_blobs.insert(hash);
            rp_it = relative_paths.erase(rp_it);
        }

        while (rp_it != relative_paths.end()) {
            const auto path = m_content_dir / *rp_it;
            m_logger.debug("Deleting {}", BS::StringUtil::quoted(rp_it->string()));
            std::filesystem::remove(path);
            rp_it = relative_paths.erase(rp_it);
        }

        it = m_content_files.erase(it);
    }

    // Cleaning content directory
    for (auto const& child_entry : std::filesystem::directory_iterator(m_content_dir)) {
        if (child_entry.is_directory() && child_entry.path().filename() != ".cache") {
            BS::IOUtil::remove_empty_directories_recursively(child_entry);
        }
    }

    // Restoring from cache
    for (auto const& [wd_hash, wd_relative_paths]: repository.workdir_files()) {

        if (m_content_files.contains(wd_hash))
            continue;

        if (!m_cached_blobs.contains(wd_hash)) {

            for (const auto &wd_relative_path: wd_relative_paths) {
                m_logger.warning("Unable to restore {}", BS::StringUtil::quoted(wd_relative_path));
            }

            continue;
        }

        auto wd_it = wd_relative_paths.begin();

        const auto first_path = m_content_dir / *wd_it;

        {
            const auto from_path = blobs_dir() / Encoding::encode_hex_upper(wd_hash);
            m_logger.debug("Restoring {}", BS::StringUtil::quoted(wd_it->string()));
            std::filesystem::create_directories(first_path.parent_path());
            std::filesystem::rename(from_path, first_path);
            ++wd_it;
        }

        for (; wd_it != wd_relative_paths.end(); ++wd_it) {
            const auto to_path = m_content_dir / *wd_it;
            m_logger.debug("Copying {}", BS::StringUtil::quoted(wd_it->string()));
            std::filesystem::create_directories(to_path.parent_path());
            std::filesystem::copy(first_path, to_path);
        }
    }
}
