#include "ContentFileCache.h"

#include <vector>

#include "Base/BitUtil.h"
#include "Base/Encoding.h"
#include "Base/Crypto.h"

using last_hit_type = int32_t;

constexpr std::size_t fingerprint_size = 28;
constexpr std::size_t last_hit_size = sizeof(last_hit_type);
constexpr std::size_t file_hash_size = 32;

std::map<std::vector<uint8_t>, BS::App::ContentFileCacheEntry> BS::App::ContentFileCache::load(std::istream &input) {

    std::map<std::vector<uint8_t>, BS::App::ContentFileCacheEntry> result;

    std::array<uint8_t, last_hit_size + fingerprint_size + file_hash_size> buffer {};

    while (input.read(reinterpret_cast<char *>(buffer.data()), buffer.size()) &&
           input.gcount() == buffer.size())
    {
        auto span = std::span<const uint8_t>(buffer.begin(), 0);

        span = std::span(span.end(), fingerprint_size);
        const auto fingerprint = std::vector(span.begin(), span.end());

        span = std::span(span.end(), last_hit_size);
        const auto last_hit_time_bytes = std::vector(span.begin(), span.end());

        span = std::span(span.end(), file_hash_size);
        const auto file_hash = std::vector(span.begin(), span.end());

        BS::App::ContentFileCacheEntry entry;

        entry.set_file_hash(file_hash);
        entry.set_last_hit_time_minutes(
            std::chrono::time_point<std::chrono::system_clock>(
                std::chrono::minutes(
                    BitUtil::from_bytes_be<last_hit_type>(last_hit_time_bytes))));

        result[fingerprint] = entry;
    }

    return result;
}

void BS::App::ContentFileCache::save(std::ostream &output, const std::map<std::vector<uint8_t>, BS::App::ContentFileCacheEntry> &map) {

    for (const auto &[fingerprint, entry]: map) {

        const auto last_hit_time_bytes =
            BitUtil::to_bytes_be<last_hit_type>(
                static_cast<last_hit_type>(
                    std::chrono::duration_cast<std::chrono::minutes>(
                        entry.last_hit_time_minutes().time_since_epoch()).count()));

        output.write(reinterpret_cast<const char *>(fingerprint.data()), static_cast<std::streamsize>(fingerprint.size()));
        output.write(reinterpret_cast<const char *>(last_hit_time_bytes.data()), static_cast<std::streamsize>(last_hit_time_bytes.size()));
        output.write(reinterpret_cast<const char *>(entry.file_hash().data()), static_cast<std::streamsize>(entry.file_hash().size()));
    }
}

std::vector<uint8_t> BS::App::ContentFileCache::compute_fingerprint(const BS::App::ContentFileInfo &fi) {

    const auto nanoseconds_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::file_clock::to_sys(fi.modify_time()).time_since_epoch()).count();

    const auto relative_path_str = fi.relative_path().string();
    const auto file_size_bytes = BS::BitUtil::to_bytes_be<uint64_t>(fi.file_size());
    const auto file_time_bytes = BS::BitUtil::to_bytes_be<int64_t>(nanoseconds_since_epoch);

    std::vector<uint8_t> data;

    data.reserve(
            relative_path_str.size() +
            file_size_bytes.size() +
            file_time_bytes.size());

    data.insert(data.end(), relative_path_str.begin(), relative_path_str.end());
    data.insert(data.end(), file_size_bytes.begin(), file_size_bytes.end());
    data.insert(data.end(), file_time_bytes.begin(), file_time_bytes.end());

    return BS::Crypto::compute_sha_224(data);
}
