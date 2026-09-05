#include "cli_options.hpp"

#include <iostream>
#include <string_view>

namespace cli {

namespace {

void print_usage(std::string_view program_name) {
    std::cout << "Usage: " << program_name << " [books_file]\n";
    std::cout << "       " << program_name << " -h\n";
    std::cout << "       " << program_name << " -v\n";
    std::cout << "       " << program_name << " --check [books_file]\n";
    std::cout << "       " << program_name << " --list [books_file]\n";
}

void print_version(std::string_view program_name) {
    std::cout << program_name << " version " << LIBRARY_CLI_VERSION << std::endl;
}

} // namespace

StartupAction parse_command_line(int argc, char* argv[], std::string& file_path) {
    const std::string_view option = argc >= 2 ? argv[1] : "";

    if (argc == 2 && (option == "--help" || option == "-h")) {
        print_usage(argv[0]);
        return StartupAction::exit_success;
    }

    if (argc == 2 && (option == "--version" || option == "-v")) {
        print_version(argv[0]);
        return StartupAction::exit_success;
    }

    if (option == "--check") {
        if (argc == 2) {
            file_path = "books.txt";
        } else if (argc == 3) {
            file_path = argv[2];
        } else {
            std::cerr << "Too many command line arguments.\n";
            print_usage(argv[0]);
            return StartupAction::exit_failure;
        }

        return StartupAction::check;
    }

    if (option == "--list") {
        if (argc == 2) {
            file_path = "books.txt";
        } else if (argc == 3) {
            file_path = argv[2];
        } else {
            std::cerr << "Too many command line arguments.\n";
            print_usage(argv[0]);
            return StartupAction::exit_failure;
        }
        return StartupAction::list;
    }

    if (argc == 2 && option.empty()) {
        std::cerr << "Books file path cannot be empty.\n";
        print_usage(argv[0]);
        return StartupAction::exit_failure;
    }

    if (argc == 2 && option.compare(0, 2, "--") == 0) {
        std::cerr << "Unknown option: " << option << '\n';
        print_usage(argv[0]);
        return StartupAction::exit_failure;
    }

    if (argc > 2) {
        std::cerr << "Too many command line arguments.\n";
        print_usage(argv[0]);
        return StartupAction::exit_failure;
    }

    file_path = option.empty() ? "books.txt" : std::string(option);
    return StartupAction::run;
}

} // namespace cli