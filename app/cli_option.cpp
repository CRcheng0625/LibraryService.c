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
    std::cout << "       " << program_name << " --count [books_file]\n";
    std::cout << "       " << program_name << " --stats [books_file]\n";
    std::cout << "       " << program_name << " --available [books_file]\n";
    std::cout << "       " << program_name << " --search-title <keyword> [books_file]\n";
    std::cout << "       " << program_name << " --search-author <keyword> [books_file]\n";
}

void print_version(std::string_view program_name) {
    std::cout << program_name << " version " << LIBRARY_CLI_VERSION << '\n';
}

bool parse_optional_file_path(int argc, char* argv[], std::string& file_path) {
    if (argc == 2) {
        file_path = "books.txt";
        return true;
    }
    if (argc == 3) {
        file_path = argv[2];
        return true;
    }
    return false;
}

// The two output strings are intentionally paired: the parser fills both for main().
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
StartupAction parse_search_command(int argc, char* argv[], std::string& file_path,
                                   std::string& search_keyword, std::string_view option) {
    if (argc != 3 && argc != 4) {
        std::cerr << "Expected a keyword and optional books file.\n";
        print_usage(argv[0]);
        return StartupAction::exit_failure;
    }

    search_keyword = argv[2];
    if (search_keyword.empty()) {
        std::cerr << "Search keyword cannot be empty.\n";
        return StartupAction::exit_failure;
    }

    file_path = argc == 4 ? argv[3] : "books.txt";

    if (option == "--search-title") {
        return StartupAction::search_title;
    }
    return StartupAction::search_author;
}

} // namespace

// NOLINTNEXTLINE(readability-function-cognitive-complexity, bugprone-easily-swappable-parameters)
StartupAction parse_command_line(int argc, char* argv[], std::string& file_path,
                                 std::string& search_keyword) {

    const std::string_view option = argc >= 2 ? argv[1] : "";

    if (argc == 2 && (option == "--help" || option == "-h")) {
        print_usage(argv[0]);
        return StartupAction::exit_success;
    }

    if (argc == 2 && (option == "--version" || option == "-v")) {
        print_version(argv[0]);
        return StartupAction::exit_success;
    }

    if (option == "--check" || option == "--list" || option == "--count" || option == "--stats" ||
        option == "--available") {
        if (!parse_optional_file_path(argc, argv, file_path)) {
            std::cerr << "Too many command line arguments.\n";
            print_usage(argv[0]);
            return StartupAction::exit_failure;
        }
        if (option == "--check") {
            return StartupAction::check;
        }
        if (option == "--list") {
            return StartupAction::list;
        }
        if (option == "--stats") {
            return StartupAction::stats;
        }
        if (option == "--available") {
            return StartupAction::available;
        }
        return StartupAction::count;
    }

    if (option == "--search-title" || option == "--search-author") {
        return parse_search_command(argc, argv, file_path, search_keyword, option);
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
