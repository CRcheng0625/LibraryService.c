#include "library/book_storage.hpp"

#include <fstream>
#include <iomanip>
#include <vector>

namespace library {

    bool save_books_to_file(
        const std::vector<Book>& books,
        const std::string& file_path) {

        std::ofstream output(file_path);

        if (!output) {
            return false;
        }

        for (const Book& book : books) {
            output << book.id << ' '
                << std::quoted(book.title) << ' '
                << std::quoted(book.author) << ' '
                << book.publication_year << ' '
                << book.borrowed << '\n';
        }

        return static_cast<bool>(output);
    }

    std::vector<Book> load_books_from_file(
        const std::string& file_path) {

        std::ifstream input(file_path);

        if (!input) {
            return {};
        }

        std::vector<Book> books;
        Book book;

        while (input >> book.id
            >> std::quoted(book.title)
            >> std::quoted(book.author)
            >> book.publication_year
            >> book.borrowed) {
            books.push_back(book);
        }

        if (!input.eof()) {
            return {};
        }

        return books;
    }

}  // namespace library