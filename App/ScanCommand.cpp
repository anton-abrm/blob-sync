#include "ScanCommand.h"

#include <iostream>
#include <filesystem>
#include <utility>

#include "App/Config.h"
#include "App/ContentStorage.h"
#include "App/Repository.h"

void BS::App::ScanCommand::execute(BS::App::Logger &logger) {

    logger.info("Scan started");

    BS::App::ContentStorage content_storage(logger, config().content_dir());

    content_storage.scan_content_files();

    if (m_deduplicationMode) {

        switch (m_deduplicationMode.value()) {

            case DeduplicationMode::PreserveFirst:
                logger.info("Deduplication: Preserve First");
                content_storage.remove_duplicates_preserve_first();
                break;

            case DeduplicationMode::PreserveLast:
                logger.info("Deduplication: Preserve Last");
                content_storage.remove_duplicates_preserve_last();
                break;
        }
    }

    BS::App::Repository repository(logger, repository_directory());

    repository.scan_workdir_files();

    repository.restore_from(content_storage);

    logger.info("Scan finished");
}

BS::App::ScanCommand::ScanCommand(
    const std::filesystem::path& repository_directory,
    const BS::App::CommandArguments &args)
    : Command(repository_directory, args) {

    if (args.command_arguments().contains("u")) {
        m_deduplicationMode = DeduplicationMode::PreserveFirst;
    }

    if (args.command_arguments().contains("U")) {
        m_deduplicationMode = DeduplicationMode::PreserveLast;
    }
}

void BS::App::ScanCommand::print_usage() {
    std::cout << "Usage:\n"
                 "    blob-sync scan [command options]\n"
                 "\n"
                 "Command options:\n"
                 "        -u    Deduplicate files, preserve only the first occurrence\n"
                 "        -U    Deduplicate files, preserve only the last occurrence\n";
}
