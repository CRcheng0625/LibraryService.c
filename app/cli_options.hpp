#pragma once

#include <string>

namespace cli {

enum class StartupAction {
    run,
    check,
    exit_success,
    exit_failure
};

StartupAction parse_command_line(int argc, char* argv[], std::string& file_path);

} // namespace cli