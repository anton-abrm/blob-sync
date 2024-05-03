#pragma once

#include <stdexcept>

namespace BS::Exceptions {
    class InvalidUsageException final : public virtual std::runtime_error {
    public:
        explicit InvalidUsageException() : runtime_error(std::string()) {}
    };
}


