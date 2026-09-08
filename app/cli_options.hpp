#pragma once

#include <string>

namespace cli {

enum class StartupAction {
    run,
    check,
    list,
    count,
    stats,
    available,
    mysql,
    search_title,
    search_author,
    exit_success,
    exit_failure
};

StartupAction parse_command_line(int argc, char* argv[], std::string& file_path,
    std::string& search_keyword);

} // namespace cli
