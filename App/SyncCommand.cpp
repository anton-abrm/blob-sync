#include "SyncCommand.h"

#include <iostream>
#include <filesystem>

#include "App/Config.h"
#include "App/ContentStorage.h"
#include "App/Repository.h"


void BS::App::SyncCommand::execute() {

    logger().info("Sync started");

    const auto repository_path = working_directory();

    const auto config = BS::App::Config::load(repository_path);

    BS::App::Repository repository(logger(), repository_path);

    repository.scan_committed_blobs();
    repository.scan_workdir_files();

    BS::App::ContentStorage content_storage(logger(), config.content_dir(), config.storage_url());

    content_storage.scan_cached_blobs();
    content_storage.scan_content_files();
    content_storage.scan_remote_blobs();

    content_storage.download_blobs(repository);
    content_storage.upload_blobs(repository, m_prune);
    content_storage.restore_from(repository);

    logger().info("Sync finished");
}

BS::App::SyncCommand::SyncCommand(std::shared_ptr<BS::App::Logger> logger, const BS::App::CommandArguments &args)
    : Command(std::move(logger), args) {

    if (args.command_arguments().contains("prune")) {
        m_prune = true;
    }
}

void BS::App::SyncCommand::print_usage() {

    std::cout << "Usage:\n"
                 "    blob-sync sync [command options]\n"
                 "\n"
                 "Command options:\n"
                 "        --prune   Remove uncommitted blobs from the remote\n";
}
