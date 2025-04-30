#include "GitConfig.h"

#include <format>
#include <fstream>
#include <Base/StringUtil.h>

std::string create_not_found_message(const std::string& section, const std::string& key, const std::string& sub_section) {
    return sub_section.empty()
        ? std::format("Unable to get the config value for '{}.{}'", section, key)
        : std::format("Unable to get the config value for '{}.{}' in '{}'", section, key, sub_section);
}

void BS::GitConfig::merge(const std::filesystem::path& path) {

    std::ifstream input(path);

    if (!input)
        return;

    std::string section;
    std::string sub_section;

    std::vector<char> buffer(1024);

    while (true) {

        if (!input.getline(buffer.data(), static_cast<std::streamsize>(buffer.size())))
            break;

        std::string s(buffer.data());

        const auto comment_pos = s.find('#');

        if (comment_pos != std::string::npos) {
            s = s.substr(0, comment_pos);
        }

        s = BS::StringUtil::trim(s);

        if (s.empty())
            continue;

        if (s.starts_with('['))
        {
            if (!s.ends_with(']'))
                throw std::runtime_error("Invalid config.");

            section = StringUtil::trim(std::string(s.cbegin() + 1, s.cend() - 1));
            sub_section = std::string();

            const auto start = section.find_first_of('\"');

            if (start != std::string::npos) {

                const auto end = section.find_last_of('\"');

                if (start == end) {
                    throw std::runtime_error("Unable to read configuration.");
                }

                sub_section = section.substr(start + 1, end - start - 1);
                section = StringUtil::trim(section.substr(0, start));
            }

            continue;
        }

        const auto pos = s.find('=');

        if (pos == std::string::npos)
            throw std::runtime_error("Unable to read configuration.");

        const std::string key = BS::StringUtil::trim(s.substr(0, pos));
        const std::string val = BS::StringUtil::trim(s.substr(pos + 1));

        m_map[section][sub_section][key] = val;
    }

    if (input.fail() && !input.eof())
        throw std::runtime_error("Unable to read configuration.");
}

std::optional<bool> BS::GitConfig::get_bool(const std::string& section, const std::string& key, const std::string& sub_section) const {

    const auto string_value = get_string(section, key, sub_section);

    if (!string_value)
        return {};

    if (string_value.value() == "true") {
        return true;
    }

    if (string_value.value() == "false") {
        return false;
    }

    throw std::runtime_error("Unable to get configuration value.");
}

bool BS::GitConfig::get_required_bool(const std::string& section, const std::string& key, const std::string& sub_section) const {
    const auto value_opt = get_bool(section, key, sub_section);
    if (!value_opt)
        throw std::runtime_error(create_not_found_message(section, key, sub_section));

    return value_opt.value();
}

std::optional<std::string> BS::GitConfig::get_string(const std::string& section, const std::string& key, const std::string& sub_section) const {

    if (!m_map.contains(section))
        return {};

    const auto& section_map = m_map.at(section);

    if (!section_map.contains(sub_section))
        return {};

    const auto& sub_section_map = section_map.at(sub_section);

    if (!sub_section_map.contains(key)) {
        return {};
    }

    return sub_section_map.at(key);
}

std::string BS::GitConfig::get_required_string(const std::string& section, const std::string& key, const std::string& sub_section) const {
    const auto value_opt = get_string(section, key, sub_section);
    if (!value_opt)
        throw std::runtime_error(create_not_found_message(section, key, sub_section));

    return value_opt.value();
}

std::unordered_set<std::string> BS::GitConfig::get_sub_sections(const std::string& section) const {

    if (!m_map.contains(section))
        return {};

    std::unordered_set<std::string> sub_sections;

    for (const auto& [sub_section, sub_section_map]: m_map.at(section)) {
        sub_sections.insert(sub_section);
    }

    return sub_sections;
}
