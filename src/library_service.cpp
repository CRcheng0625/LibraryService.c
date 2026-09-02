#include "library/library_service.hpp"
#include "library/book_validation.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iterator>
#include <string>
#include <unordered_map>
#include <utility>

namespace library {
namespace {

std::string to_lower(std::string_view text) {
    std::string result(text);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return result;
}

} // namespace

void LibraryService::rebuild_index() {
    id_index_.clear();

    for (std::size_t index = 0; index < books_.size(); ++index) {
        id_index_[books_[index].id] = index;
    }
}

bool LibraryService::add_book(Book book) {
    if (!is_valid_book(book) || id_index_.find(book.id) != id_index_.end()) {
        return false;
    }

    books_.push_back(std::move(book));
    id_index_[books_.back().id] = books_.size() - 1;
    return true;
}

bool LibraryService::remove_book(int id) {
    const auto old_size = books_.size();
    books_.erase(std::remove_if(books_.begin(), books_.end(),
                                [id](const Book& book) { return book.id == id; }),
                 books_.end());
    const bool removed = books_.size() != old_size;
    if (removed) {
        rebuild_index();
    }
    return removed;
}

LoanResult LibraryService::borrow_book(int id) {
    const auto index_it = id_index_.find(id);
    if (index_it == id_index_.end() || index_it->second >= books_.size()) {
        return LoanResult::book_not_found;
    }

    Book& book = books_[index_it->second];
    if (book.borrowed) {
        return LoanResult::already_borrowed;
    }

    book.borrowed = true;
    return LoanResult::success;
}

LoanResult LibraryService::return_book(int id) {
    const auto index_it = id_index_.find(id);
    if (index_it == id_index_.end() || index_it->second >= books_.size()) {
        return LoanResult::book_not_found;
    }

    Book& book = books_[index_it->second];
    if (!book.borrowed) {
        return LoanResult::not_borrowed;
    }

    book.borrowed = false;
    return LoanResult::success;
}

bool LibraryService::update_book(int id, std::string title, std::string author,
                                 int publication_year) {
    if (title.empty() || author.empty()) {
        return false;
    }

    const auto index_it = id_index_.find(id);

    if (index_it == id_index_.end() || index_it->second >= books_.size()) {
        return false;
    }

    Book& book = books_[index_it->second];

    book.title = std::move(title);
    book.author = std::move(author);
    book.publication_year = publication_year;

    return true;
}

std::optional<Book> LibraryService::find_by_id(int id) const {
    const auto index_it = id_index_.find(id);
    if (index_it == id_index_.end()) {
        return std::nullopt;
    }

    if (index_it->second >= books_.size()) {
        return std::nullopt;
    }

    return books_[index_it->second];
}

std::vector<Book> LibraryService::search_by_title(std::string_view keyword) const {
    BookFilter filter;
    filter.title_keyword = std::string(keyword);
    return filter_books(filter);
}

std::vector<Book> LibraryService::search_by_author(std::string_view keyword) const {
    BookFilter filter;
    filter.author_keyword = std::string(keyword);
    return filter_books(filter);
}

std::vector<Book> LibraryService::search_by_year(int year) const {
    BookFilter filter;
    filter.publication_year = year;
    return filter_books(filter);
}

std::vector<Book> LibraryService::books_sorted_by_year() const {
    std::vector<Book> result = books_;
    std::stable_sort(result.begin(), result.end(), [](const Book& left, const Book& right) {
        return left.publication_year < right.publication_year;
    });
    return result;
}

std::vector<Book> LibraryService::books_sorted_by_title() const {
    std::vector<Book> result = books_;
    std::sort(result.begin(), result.end(), [](const Book& left, const Book& right) {
        return left.title != right.title ? left.title < right.title : left.id < right.id;
    });
    return result;
}

const std::vector<Book>& LibraryService::all_books() const noexcept {
    return books_;
}

std::vector<Book> LibraryService::available_books() const {
    BookFilter filter;
    filter.borrowed = false;
    return filter_books(filter);
}

std::vector<Book> LibraryService::search_by_author_and_year(std::string_view author,
    int year) const {
    BookFilter filter;
    filter.author_keyword = std::string(author);
    filter.publication_year = year;
    return filter_books(filter);
}

std::vector<Book> LibraryService::search_by_borrowed(bool borrowed) const {
    BookFilter filter;
    filter.borrowed = borrowed;
    return filter_books(filter);
}

std::vector<Book> LibraryService::search_by_author_and_borrowed(std::string_view author,
                                                                bool borrowed) const {
    BookFilter filter;
    filter.author_keyword = std::string(author);
    filter.borrowed = borrowed;
    return filter_books(filter);
}

BookPage LibraryService::books_page(std::size_t offset, std::size_t limit) const {

    BookPage page;
    page.total_books = books_.size();

    if (offset >= books_.size() || limit == 0) {
        return page;
    }

    const std::size_t available = books_.size() - offset;
    const std::size_t count = std::min(limit, available);

    page.books = std::vector<Book>(books_.begin() + offset, books_.begin() + offset + count);

    page.has_next = count < available;

    return page;
}

std::vector<Book> LibraryService::filter_books(const BookFilter& filter) const {

    std::optional<std::string> normalized_title;
    if (filter.title_keyword.has_value()) {
        normalized_title = to_lower(*filter.title_keyword);
    }

    std::optional<std::string> normalized_author;
    if (filter.author_keyword.has_value()) {
        normalized_author = to_lower(*filter.author_keyword);
    }

    std::vector<Book> matches;

    std::copy_if(books_.begin(), books_.end(),
        std::back_inserter(matches),
                 [&filter, &normalized_title, &normalized_author](const Book& book) {
                     if (normalized_title.has_value() &&
                         to_lower(book.title).find(*normalized_title) == std::string::npos) {
                         return false;
                     }
                     if (normalized_author.has_value() &&
                         to_lower(book.author).find(*normalized_author) == std::string::npos) {
                         return false;
                     }
                     if (filter.publication_year.has_value() &&
                         book.publication_year != *filter.publication_year) {
                         return false;
                     }
                     if (filter.borrowed.has_value() &&
                         book.borrowed != *filter.borrowed) {
                         return false;
                     }
                     return true;
                 });
    return matches;
}

} // namespace library
