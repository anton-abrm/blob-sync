#include <iostream>
#include <mutex>
#include <sstream>

#include "App/Command.h"
#include "App/CommandArguments.h"
#include "App/ConsoleLogger.h"

#include "Version.h"

void print_version() {

    std::stringstream ss;

    ss  << "Blob Sync" << " v"
        << CMAKE_PROJECT_VERSION
        << "."
        << GIT_COMMITS
        << "+"
        << GIT_HASH
        << GIT_DIRTY;

    std::cout << ss.str() << std::endl;
}

static void print_usage() {
    std::cout << "Usage:\n"
                 "    blob-sync [general options] <command> [command options]\n"
                 "\n"
                 "General options:\n"
                 "    -v, --verbose     Print debug information\n"
                 "    -h, --help        Print help information\n"
                 "\n"
                 "Commands:\n"
                 "    scan      Incorporates changes from the content directory into the source\n"
                 "    sync      Synchronises the content directory with the remote according to the source\n";
}

int main(int argc, const char **argv) {

    print_version();

    BS::App::CommandArguments args(argc, argv);

    if (args.general_arguments().contains("h") ||
        args.general_arguments().contains("help")) {
            print_usage();
            return 0;
    }

    BS::App::ConsoleLogger logger;

    if (args.general_arguments().contains("v") ||
        args.general_arguments().contains("verbose")) {
            logger.set_min_level(BS::App::LogLevel::Debug);
    }

    try {
        BS::App::Command::create(std::filesystem::current_path(), args)->run(logger);
    }
    catch (const std::runtime_error &ex) {
        logger.error(ex.what());
        return 1;
    }

    return 0;
}
