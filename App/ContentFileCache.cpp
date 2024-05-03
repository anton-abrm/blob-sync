#include "ContentFileCache.h"

#include <vector>

#include "Base/BitUtil.h"
#include "Base/Encoding.h"
#include "Base/Crypto.h"

std::map<std::vector<uint8_t>, std::vector<uint8_t>> BS::App::ContentFileCache::load(std::istream &input) {

    std::map<std::vector<uint8_t>, std::vector<uint8_t>> result;

    std::vector<uint8_t> buffer(64);
    while (input.read(reinterpret_cast<char *>(buffer.data()), static_cast<std::streamsize>(buffer.size())) &&
           input.gcount() == buffer.size())
    {
        const auto key = std::vector<uint8_t>(buffer.begin(), buffer.begin() + 32);
        const auto val = std::vector<uint8_t>(buffer.begin() + 32, buffer.end());

        result[key] = val;
    }

    return result;
}

void BS::App::ContentFileCache::save(std::ostream &output, const std::map<std::vector<uint8_t>, std::vector<uint8_t>> &map) {
    for (const auto &[key, val]: map) {
        output.write(reinterpret_cast<const char *>(key.data()), static_cast<std::streamsize>(key.size()));
        output.write(reinterpret_cast<const char *>(val.data()), static_cast<std::streamsize>(val.size()));
    }
}

std::vector<uint8_t> BS::App::ContentFileCache::compute_fingerprint(const BS::App::ContentFileInfo &fi) {

    const std::string relative_path_str = fi.relative_path().string();
    const std::vector<uint8_t> file_size_bytes = BS::BitUtil::to_bytes_be(fi.file_size());
    const std::vector<uint8_t> file_time_bytes = BS::BitUtil::to_bytes_be(
        static_cast<uint64_t>(fi.modify_time().count()));

    std::vector<uint8_t> data;

    data.reserve(
            relative_path_str.size() +
            file_size_bytes.size() +
            file_time_bytes.size());

    data.insert(data.end(), relative_path_str.begin(), relative_path_str.end());
    data.insert(data.end(), file_size_bytes.begin(), file_size_bytes.end());
    data.insert(data.end(), file_time_bytes.begin(), file_time_bytes.end());

    return BS::Crypto::compute_sha_256(data);
}
