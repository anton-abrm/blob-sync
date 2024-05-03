#pragma once

#include <vector>
#include <span>
#include <string>
#include <unordered_map>

namespace BS::App {
    class CommandArguments {
    public:
        explicit CommandArguments();

        [[nodiscard]] static CommandArguments parse(std::span<std::string_view> arguments);
        [[nodiscard]] static CommandArguments parse(int argc, const char **argv);

        [[nodiscard]] const std::string & command() const { return m_command; }
        [[nodiscard]] const std::unordered_map<std::string, std::string> & general_arguments() const { return m_general_arguments; };
        [[nodiscard]] const std::unordered_map<std::string, std::string> & command_arguments() const { return m_command_arguments; };
        [[nodiscard]] const std::vector<std::string> & positioned_arguments() const { return m_positioned_arguments; };

    private:
        std::string m_command;
        std::unordered_map<std::string, std::string> m_general_arguments;
        std::unordered_map<std::string, std::string> m_command_arguments;
        std::vector<std::string> m_positioned_arguments;
    };
}
