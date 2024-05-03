#pragma once

#include "App/Command.h"

namespace BS::App {
    class SyncCommand final : public virtual BS::App::Command {
    public:

        explicit SyncCommand(std::shared_ptr<BS::App::Logger> logger, const BS::App::CommandArguments &args);

        void print_usage() override;

    protected:
        void execute() override;

    private:
        bool m_prune {false};
    };
}