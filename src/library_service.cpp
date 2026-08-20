#include "library/library_service.hpp"

#include <algorithm>
#include <cctype>
#include <iterator>
#include <string>
#include <utility>

namespace library {
namespace {

std::string to_lower(std::string_view text) {
    std::string result(text);
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return result;
}

}  // namespace

bool LibraryService::add_book(Book book) {
    if (book.id <= 0 || book.title.empty() || find_by_id(book.id).has_value()) {
        return false;
    }

    books_.push_back(std::move(book));
    return true;
}

bool LibraryService::remove_book(int id) {
    const auto old_size = books_.size();
    books_.erase(
        std::remove_if(books_.begin(), books_.end(), [id](const Book& book) {
            return book.id == id;
        }),
        books_.end());
    return books_.size() != old_size;
}

bool LibraryService::borrow_book(int id) {
    auto it = std::find_if(books_.begin(), books_.end(), [id](const Book& book) {
        return book.id == id;
    });
    if (it == books_.end() || it->borrowed) {
        return false;
    }

    it->borrowed = true;
    return true;
}

bool LibraryService::return_book(int id) {
    auto it = std::find_if(books_.begin(), books_.end(), [id](const Book& book) {
        return book.id == id;
    });
    if (it == books_.end() || !it->borrowed) {
        return false;
    }

    it->borrowed = false;
    return true;
}

std::optional<Book> LibraryService::find_by_id(int id) const {
    const auto it = std::find_if(books_.begin(), books_.end(), [id](const Book& book) {
        return book.id == id;
    });
    if (it == books_.end()) {
        return std::nullopt;
    }
    return *it;
}

std::vector<Book> LibraryService::search_by_title(std::string_view keyword) const {
    const std::string normalized_keyword = to_lower(keyword);
    std::vector<Book> matches;

    std::copy_if(books_.begin(), books_.end(), std::back_inserter(matches),
                 [&normalized_keyword](const Book& book) {
                     return to_lower(book.title).find(normalized_keyword) != std::string::npos;
                 });
    return matches;
}

std::vector<Book> LibraryService::search_by_author(
    std::string_view keyword) const {
    const std::string normalized_keyword = to_lower(keyword);
    std::vector<Book> matches;

    std::copy_if(
        books_.begin(),
        books_.end(),
        std::back_inserter(matches),
        [&normalized_keyword](const Book& book) {
            return to_lower(book.author).find(normalized_keyword)
                != std::string::npos;
        });

    return matches;
}

std::vector<Book> LibraryService::books_sorted_by_year() const {
    std::vector<Book> result = books_;
    std::stable_sort(result.begin(), result.end(), [](const Book& left, const Book& right) {
        return left.publication_year < right.publication_year;
    });
    return result;
}

const std::vector<Book>& LibraryService::all_books() const noexcept {
    return books_;
}

std::vector<Book> LibraryService::available_books() const {
    std::vector<Book> result;
    for (const auto& book : books_) {
        if (!book.borrowed) {
            result.push_back(book);
        }
    }
    return result;
}

}  // namespace library
