#include "library/book_storage.hpp"
#include "library/book_validation.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <unordered_set>
#include <utility>

namespace library {

SaveStatus save_books_to_file(const std::vector<Book>& books, const std::string& file_path) {
    std::unordered_set<int> seen_ids;

    for (const Book& book : books) {
        if (!is_valid_book(book) || !seen_ids.insert(book.id).second) {
            return SaveStatus::invalid_book;
        }
    }

    std::ofstream output(file_path);

    if (!output) {
        return SaveStatus::open_error;
    }

    for (const Book& book : books) {
        output << book.id << ' ' << std::quoted(book.title) << ' ' << std::quoted(book.author)
               << ' ' << book.publication_year << ' ' << book.borrowed << '\n';
    }

    return output ? SaveStatus::success : SaveStatus::write_error;
}

LoadResult load_books_from_file(const std::string& file_path) {

    std::ifstream input(file_path);

    if (!input) {
        if (std::filesystem::exists(file_path)) {
            return {{}, LoadStatus::open_error};
        }

        return {{}, LoadStatus::file_not_found};
    }

    std::vector<Book> books;
    std::unordered_set<int> seen_ids;
    Book book;

    while (input >> book.id >> std::quoted(book.title) >> std::quoted(book.author) >>
           book.publication_year >> book.borrowed) {
        if (!is_valid_book(book)) {
            return {{}, LoadStatus::invalid_format};
        }

        if (!seen_ids.insert(book.id).second) {
            return {{}, LoadStatus::invalid_format};
        }

        books.push_back(book);
    }

    if (!input.eof()) {
        return {{}, LoadStatus::invalid_format};
    }

    return {std::move(books), LoadStatus::success};
}

} // namespace library
