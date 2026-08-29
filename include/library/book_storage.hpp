#pragma once

#include "library/book.hpp"

#include <string>
#include <vector>

namespace library {

enum class LoadStatus {
    success,
    file_not_found,
    open_error,
    invalid_format
};

struct LoadResult {
    std::vector<Book> books;
    LoadStatus status{LoadStatus::success};
};

bool save_books_to_file(
    const std::vector<Book>& books,
    const std::string& file_path);

LoadResult load_books_from_file(
    const std::string& file_path);

}  // namespace library
