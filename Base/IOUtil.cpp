#include "IOUtil.h"

void BS::IOUtil::remove_empty_directories_recursively(const std::filesystem::directory_entry &entry) {

    if (!entry.is_directory())
        throw std::invalid_argument("path is not a directory.");

    for (const auto &child_entry : std::filesystem::directory_iterator(entry)) {
        if (child_entry.is_directory())
            remove_empty_directories_recursively(child_entry);
    }

    if (std::filesystem::is_empty(entry.path()))
        std::filesystem::remove(entry.path());
}
