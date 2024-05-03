#include "ScanCommand.h"

#include <iostream>
#include <filesystem>

#include "App/Config.h"
#include "App/ContentStorage.h"
#include "App/Repository.h"

void BS::App::ScanCommand::execute() {

    logger().info("Scan started");

    const auto repository_path = working_directory();

    const auto config = BS::App::Config::load(repository_path);

    BS::App::ContentStorage content_storage(logger(), config.content_dir(), config.storage_url());

    content_storage.scan_content_files();

    BS::App::Repository repository(logger(), repository_path);

    repository.scan_workdir_files();

    repository.restore_from(content_storage);

    logger().info("Scan finished");
}

BS::App::ScanCommand::ScanCommand(std::shared_ptr<BS::App::Logger> logger, const BS::App::CommandArguments &args)
    : Command(std::move(logger), args) {
}

void BS::App::ScanCommand::print_usage() {
    std::cout << "Usage:\n"
                 "    blob-sync scan [command options]\n";
}
