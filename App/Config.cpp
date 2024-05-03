#include "Config.h"

#include <vector>
#include <fstream>

#include "Base/StringUtil.h"

std::filesystem::path BS::App::Config::content_dir() const {
    return m_content_dir;
}

std::string BS::App::Config::storage_url() const {
    return m_storage_url;
}

void BS::App::Config::set_content_dir(const std::filesystem::path &value) {
    m_content_dir = value;
}

void BS::App::Config::set_storage_url(const std::string &value) {
    m_storage_url = value;
}

BS::App::Config BS::App::Config::load(const std::filesystem::path &repository_path) {

    const auto git_config_path = repository_path / ".git" / "config";

    std::ifstream input(git_config_path);

    if (!input)
        throw std::runtime_error("Unable to read configuration.");

    BS::App::Config result;

    bool in_section = false;

    std::vector<char> buffer(1024);

    while (true) {

        if (!input.getline(buffer.data(), static_cast<std::streamsize>(buffer.size())))
            break;

        std::string s(buffer.data());

        const auto comment_pos = s.find('#');

        if (comment_pos != std::string::npos) {
            s = s.substr(0, comment_pos);
        }

        if (s.starts_with('['))
        {
            if (in_section)
                break;

            if (s == "[blobsync]" || s == "[blob-sync]") {
                in_section = true;
            }

            continue;
        }

        if (!in_section)
            continue;

        const auto pos = s.find('=');

        if (pos == std::string::npos)
            continue;

        const std::string key = BS::StringUtil::trim(s.substr(0, pos));
        const std::string val = BS::StringUtil::trim(s.substr(pos + 1));

        if (key == "content-dir")
            result.set_content_dir(val);
        else if (key == "storage-url")
            result.set_storage_url(val);
    }

    if (input.fail() && !input.eof())
        throw std::runtime_error("Unable to read configuration.");

    if (result.content_dir().empty())
        throw std::runtime_error("content-dir is not set.");

    if (result.storage_url().empty())
        throw std::runtime_error("storage-url is not set.");

    return result;
}
