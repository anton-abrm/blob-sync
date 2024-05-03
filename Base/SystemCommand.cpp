#include "SystemCommand.h"

#include <array>
#include <format>

#include "Base/SystemCommandException.h"

std::vector<std::string> BS::SystemCommand::exec_lines(const std::string &command) {

    const auto pipe = popen(command.c_str(), "r");

    if (!pipe)
        throw std::runtime_error("popen() failed!");

    std::vector<std::string> result;

    std::array<char, 1024> buffer {};

    while (!feof(pipe) && fgets(buffer.data(), buffer.size(), pipe) != nullptr) {

        std::string line(buffer.data());

        if (!line.ends_with('\n'))
            throw std::runtime_error("The line exceeds the buffer length.");

        result.push_back(line.substr(0, line.size() - 1));
    }

    const auto rc = pclose(pipe);

    if (rc != EXIT_SUCCESS)
        throw BS::SystemCommandException(rc);

    return result;
}

void BS::SystemCommand::exec(const std::string &command) {

    const auto rc = std::system(command.c_str());

    if (rc != EXIT_SUCCESS)
        throw BS::SystemCommandException(rc);
}
