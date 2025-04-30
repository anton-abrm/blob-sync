#pragma once

#include <optional>
#include <string>

namespace BS::App {
    class StorageConfig {
    public:

        [[nodiscard]] std::string url() const { return m_url; }

        void set_url(const std::string& url) { m_url = url; }

    private:
        std::string m_url;
    };

}


