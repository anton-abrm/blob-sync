#include "Command.h"

#include <filesystem>

#include "App/SyncCommand.h"
#include "App/ScanCommand.h"
#include "Exceptions/InvalidUsageException.h"

BS::App::Command::~Command() = default;

std::filesystem::path BS::App::Command::working_directory() const {
    return m_working_directory;
}

BS::App::Command::Command(std::shared_ptr<BS::App::Logger> logger, const BS::App::CommandArguments &args)
        : m_logger(std::move(logger))
{
    if (args.command_arguments().contains("h") ||
        args.command_arguments().contains("help")) {
        m_print_usage = true;
    }

    m_working_directory = std::filesystem::current_path();
}

std::unique_ptr<BS::App::Command> BS::App::Command::create(
    std::shared_ptr<BS::App::Logger> logger,
    const BS::App::CommandArguments &args) {

    if (args.command() == "scan")
        return std::make_unique<BS::App::ScanCommand>(std::move(logger), args);

    if (args.command() == "sync")
        return std::make_unique<BS::App::SyncCommand>(std::move(logger), args);

    throw BS::Exceptions::InvalidUsageException();
}

void BS::App::Command::run() {

    if (m_print_usage) {
        print_usage();
        return;
    }

    execute();
}

