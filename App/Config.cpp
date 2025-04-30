#include "Config.h"

#include <fstream>

#include "Git/GitConfig.h"

BS::App::Config::Config(const std::filesystem::path &repository_path) {

    GitConfig git_config;

    git_config.merge(repository_path / ".git" / "config");
    git_config.merge(repository_path / ".git" / "config.blob-sync");

    m_content_dir = git_config.get_required_string("bs-content", "dir");

    for (const auto& sub_section: git_config.get_sub_sections("bs-storage")) {

        StorageConfig storage_config;

        storage_config.set_url(git_config.get_required_string("bs-storage", "url", sub_section));

        m_storages[sub_section] = storage_config;
    }
}
