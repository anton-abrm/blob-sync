#pragma once

#include <filesystem>
#include <memory>

#include "App/Logger.h"
#include "App/CommandArguments.h"

namespace BS::App {
   class Command {
   public:

       explicit Command(std::shared_ptr<BS::App::Logger> logger, const BS::App::CommandArguments & args);

       void run();

       static std::unique_ptr<BS::App::Command> create(
           std::shared_ptr<BS::App::Logger> logger,
           const BS::App::CommandArguments & args);

       virtual ~Command();

   protected:
       virtual void execute() = 0;
       virtual void print_usage() = 0;

       [[nodiscard]] BS::App::Logger & logger() const { return *m_logger; };
       [[nodiscard]] std::filesystem::path working_directory() const;

   private:
      std::filesystem::path m_working_directory;
      bool m_print_usage {false};

      std::shared_ptr<BS::App::Logger> m_logger;
   };
}