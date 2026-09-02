#include "library/book_validation.hpp"

namespace library {

bool is_valid_book(const Book& book) {
    return book.id > 0 &&
        !book.title.empty() &&
        !book.author.empty() &&
        book.publication_year > 0;
}

} // namespace library