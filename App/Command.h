#pragma once

#include <filesystem>
#include <memory>

#include "App/Logger.h"
#include "App/CommandArguments.h"
#include "App/Config.h"

namespace BS::App {
   class Command {
   public:

       explicit Command(
           const std::filesystem::path& repository_directory,
           const BS::App::CommandArguments & args);

       void run(BS::App::Logger &logger);

       static std::unique_ptr<BS::App::Command> create(
           const std::filesystem::path &repository_directory,
           const BS::App::CommandArguments & args);

       virtual ~Command();

   protected:
       virtual void execute(BS::App::Logger &logger) = 0;
       virtual void print_usage() = 0;

       [[nodiscard]] const std::filesystem::path &repository_directory() const { return m_repository_directory; }
       [[nodiscard]] const BS::App::Config &config() const { return m_config; }

   private:
       std::filesystem::path m_repository_directory;
       BS::App::Config m_config;
       bool m_print_usage {false};
   };
}