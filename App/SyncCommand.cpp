#include "SyncCommand.h"

#include <iostream>
#include <filesystem>
#include <utility>
#include <Base/StringUtil.h>

#include "App/Config.h"
#include "App/ContentStorage.h"
#include "App/Repository.h"

void BS::App::SyncCommand::execute(BS::App::Logger &logger) {

    logger.info("Sync started");

    BS::App::Repository repository(logger, repository_directory());

    repository.scan_workdir_files();
    repository.scan_committed_blobs();

    BS::App::ContentStorage content_storage(logger, config().content_dir());

    content_storage.scan_cached_blobs();
    content_storage.scan_content_files();

    if (m_offline || !m_storage_key && config().storages().empty()) {
        logger.info("Sync mode: Offline");
    }
    else {

        logger.info("Sync mode: Online");

        const auto format_storage_key = [](const std::string &storage_key) -> std::string {
            return storage_key.empty()
                ? "Default"
                : StringUtil::quoted(storage_key);
        };

        if (m_all) {

            for (const auto& [storage_key, storage]: config().storages()) {
                logger.info("Downloading: {}", format_storage_key(storage_key));
                content_storage.scan_remote_blobs(storage.url());
                content_storage.download_blobs(repository, storage.url());
            }

            for (const auto& [storage_key, storage]: config().storages()) {
                logger.info("Uploading: {}", format_storage_key(storage_key));
                content_storage.scan_remote_blobs(storage.url());
                content_storage.upload_blobs(repository, storage.url(), m_prune);
            }
        }
        else {
            auto storage_key = m_storage_key.value_or("");

            if (!config().storages().contains(storage_key)) {
                throw std::runtime_error("The specified storage is not found.");
            }

            auto storage = config().storages().at(storage_key);

            logger.info("Syncing: {}", format_storage_key(storage_key));

            content_storage.scan_remote_blobs(storage.url());
            content_storage.download_blobs(repository, storage.url());
            content_storage.upload_blobs(repository, storage.url(), m_prune);
        }
    }

    content_storage.restore_from(repository);

    logger.info("Sync finished");
}

BS::App::SyncCommand::SyncCommand(
    const std::filesystem::path& repository_directory,
    const BS::App::CommandArguments &args)
    : Command(repository_directory, args) {

    if (args.command_arguments().contains("prune")) {
        m_prune = true;
    }

    if (args.command_arguments().contains("offline")) {
        m_offline = true;
    }

    if (args.command_arguments().contains("all")) {
        m_all = true;
    }

    if (!args.positioned_arguments().empty()) {
        m_storage_key = args.positioned_arguments()[0];
    }
}

void BS::App::SyncCommand::print_usage() {

    std::cout << "Usage:\n"
                 "    blob-sync sync [command options]\n"
                 "\n"
                 "Command options:\n"
                 "        --prune    Remove uncommitted blobs from the remote\n"
                 "        --offline  Do not synchronise with the remote storage\n"
                 "        --all      Synchronise all remotes\n";
}
