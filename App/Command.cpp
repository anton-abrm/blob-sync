#include "Command.h"

#include <filesystem>

#include "App/SyncCommand.h"
#include "App/ScanCommand.h"

BS::App::Command::~Command() = default;

BS::App::Command::Command(
    const std::filesystem::path& repository_directory,
    const BS::App::CommandArguments &args)
        : m_repository_directory(repository_directory)
        , m_config(repository_directory)
{
    if (args.command_arguments().contains("h") ||
        args.command_arguments().contains("help")) {
        m_print_usage = true;
    }
}

std::unique_ptr<BS::App::Command> BS::App::Command::create(
    const std::filesystem::path &repository_directory,
    const BS::App::CommandArguments &args) {

    if (args.command() == "scan")
        return std::make_unique<BS::App::ScanCommand>(repository_directory, args);

    if (args.command() == "sync")
        return std::make_unique<BS::App::SyncCommand>(repository_directory, args);

    throw std::runtime_error("Invalid command.");
}

void BS::App::Command::run(BS::App::Logger &logger) {

    if (m_print_usage) {
        print_usage();
        return;
    }

    execute(logger);
}

