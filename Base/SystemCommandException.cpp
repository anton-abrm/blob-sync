#include "SystemCommandException.h"

#include <format>

BS::SystemCommandException::SystemCommandException(int64_t error_code)
    : std::runtime_error(std::format("The process finished with exit code {}.", error_code)),
      m_error_code(error_code) {
}

int64_t BS::SystemCommandException::error_code() const {
    return m_error_code;
}
