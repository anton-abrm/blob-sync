#pragma once

#include <stdexcept>

namespace BS {

    class SystemCommandException final : public virtual std::runtime_error {
    public:
        explicit SystemCommandException(int64_t error_code);

        [[nodiscard]] int64_t error_code() const;

    private:
        int64_t m_error_code;
    };

}
