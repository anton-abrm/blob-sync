#include "ContentFileInfo.h"

#include "Base/Encoding.h"

std::chrono::nanoseconds BS::App::ContentFileInfo::modify_time() const {
    return m_modify_time;
}

std::size_t BS::App::ContentFileInfo::file_size() const {
    return m_file_size;
}

void BS::App::ContentFileInfo::set_modify_time(std::chrono::nanoseconds value) {
    m_modify_time = value;
}

void BS::App::ContentFileInfo::set_file_size(std::size_t value) {
    m_file_size = value;
}

void BS::App::ContentFileInfo::set_relative_path(const std::filesystem::path &relative_path) {
    m_relative_path = relative_path;
}

std::filesystem::path BS::App::ContentFileInfo::relative_path() const {
    return m_relative_path;
}
