#include "CommandArguments.h"

#include <algorithm>

#include "Base/StringUtil.h"

BS::App::CommandArguments::CommandArguments() = default;

BS::App::CommandArguments BS::App::CommandArguments::parse(std::span<std::string_view> arguments) {

    CommandArguments result;

    for (std::span<std::string_view>::size_type i = 1; i < arguments.size(); ++i) {

        const auto & argument = arguments[i];

        if (argument.starts_with("-"))  {

            const auto begin_it = argument.starts_with("--")
                    ? argument.begin() + 2
                    : argument.begin() + 1;

            std::string_view key;
            std::string_view value;

            const auto it = std::find(begin_it, argument.end(), '=');

            key = std::string_view(begin_it, it);

            if (it != argument.end()) {
                value = std::string_view(it + 1, argument.end());
            }

            std::unordered_map<std::string, std::string> & map = result.m_command.empty()
                    ? result.m_general_arguments
                    : result.m_command_arguments;

            if (!argument.starts_with("--")) {

                for (const auto ch :key) {
                    map[std::string{ ch }];
                }

                if (!key.empty()) {
                    map[std::string{ key.back() }] = std::string{ value };
                }
            }
            else {
                map[std::string{ key }] = std::string{ value };
            }
        }
        else
        {
            if (result.m_command.empty()) {
                result.m_command = argument;
            }
            else {
                result.m_positioned_arguments.emplace_back(argument);
            }
        }
    }

    return result;
}

BS::App::CommandArguments BS::App::CommandArguments::parse(int argc, const char **argv) {

    std::vector<std::string_view> args;

    args.reserve(argc);

    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    return parse(args);
}
