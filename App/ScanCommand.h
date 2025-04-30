#pragma once

#include "App/Command.h"

namespace BS::App {
    class ScanCommand final : public virtual BS::App::Command {

    private:
        enum class DeduplicationMode {
            PreserveFirst,
            PreserveLast,
        };

    public:

        explicit ScanCommand(
            const std::filesystem::path& repository_directory,
            const BS::App::CommandArguments &args);

        void print_usage() override;

    protected:
        void execute(BS::App::Logger &logger) override;

    private:
        std::optional<DeduplicationMode> m_deduplicationMode;
    };


}