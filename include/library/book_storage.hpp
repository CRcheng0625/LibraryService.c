#pragma once

#include "library/book.hpp"

#include <string>
#include <vector>

namespace library {

    bool save_books_to_file(
        const std::vector<Book>& books,
        const std::string& file_path);

    std::vector<Book> load_books_from_file(
        const std::string& file_path);

}  // namespace library