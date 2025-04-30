#pragma once

#include "App/Command.h"

namespace BS::App {
    class SyncCommand final : public virtual BS::App::Command {
    public:

        explicit SyncCommand(
            const std::filesystem::path& repository_directory,
            const BS::App::CommandArguments &args);

        void print_usage() override;

    protected:
        void execute(BS::App::Logger &logger) override;

    private:
        bool m_prune {false};
        bool m_offline {false};
        bool m_all {false};
        std::optional<std::string> m_storage_key;
    };
}