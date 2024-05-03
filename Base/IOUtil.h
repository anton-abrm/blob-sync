#pragma once

#include <filesystem>

namespace BS {
    class IOUtil final {
    public:
        static void remove_empty_directories_recursively(const std::filesystem::directory_entry &entry);
    };
}
