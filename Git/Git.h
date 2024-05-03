#pragma once

#include <set>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <functional>

namespace BS {
    class Git final {

    private:
        explicit Git();

    public:
        static BS::Git & instance();

        void get_committed_blobs(const std::filesystem::path &tree_ptr,
                                 std::function<void(std::string_view)> action);

        Git(Git const&) = delete;
        void operator=(Git const&) = delete;

        ~Git();
    };

}

