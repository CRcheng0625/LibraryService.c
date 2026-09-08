#include "library/book_repository.hpp"

#include <mysqlx/xdevapi.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

namespace {

std::string environment_value(const char* name, const char* fallback) {
    if (const char* value = std::getenv(name)) {
        return value;
    }
    return fallback;
}

void wait_for_exit() {
    std::cout << "Press Enter to close..." << std::endl;
    std::string ignored;
    std::getline(std::cin, ignored);
}

} // namespace

int main() {
    const std::string host = environment_value("MYSQL_HOST", "127.0.0.1");
    const std::string user = environment_value("MYSQL_USER", "root");
    std::string password = environment_value("MYSQL_PASSWORD", "");
    const std::string schema = environment_value("MYSQL_SCHEMA", "library_app");

    if (password.empty()) {
        std::cout << "MySQL password: ";
        std::getline(std::cin, password);
    }

    int exit_code = 0;
    try {
        mysqlx::Session session(host, 33060, user, password, schema);
        session.sql("SELECT 1").execute();
        std::cout << "MySQL connection succeeded.\n";

        library::MySqlConnectionConfig config;
        config.host = host;
        config.port = 33060;
        config.user = user;
        config.password = password;
        config.schema = schema;
        library::MySqlBookRepository repository(std::move(config));
        const auto result = repository.load();
        if (result.status != library::LoadStatus::success) {
            std::cerr << "Could not load books table.\n";
            exit_code = 1;
        } else {
            std::cout << "Loaded " << result.books.size() << " book(s) from MySQL.\n";
        }
    } catch (const mysqlx::Error& error) {
        std::cerr << "MySQL connection failed: " << error.what() << '\n';
        exit_code = 1;
    }

    wait_for_exit();
    return exit_code;
}
