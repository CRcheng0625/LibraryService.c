#include "library/library_service.hpp"

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
    if (book.id <= 0 || book.title.empty() || id_index_.find(book.id) != id_index_.end()) {
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

bool LibraryService::borrow_book(int id) {
    const auto index_it = id_index_.find(id);
    if (index_it == id_index_.end() || index_it->second >= books_.size()) {
        return false;
    }

    Book& book = books_[index_it->second];
    if (book.borrowed) {
        return false;
    }

    book.borrowed = true;
    return true;
}

bool LibraryService::return_book(int id) {
    const auto index_it = id_index_.find(id);
    if (index_it == id_index_.end() || index_it->second >= books_.size()) {
        return false;
    }

    Book& book = books_[index_it->second];
    if (!book.borrowed) {
        return false;
    }

    book.borrowed = false;
    return true;
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
    const std::string normalized_keyword = to_lower(keyword);
    std::vector<Book> matches;

    std::copy_if(books_.begin(), books_.end(), std::back_inserter(matches),
                 [&normalized_keyword](const Book& book) {
                     return to_lower(book.title).find(normalized_keyword) != std::string::npos;
                 });
    return matches;
}

std::vector<Book> LibraryService::search_by_author(std::string_view keyword) const {
    const std::string normalized_keyword = to_lower(keyword);
    std::vector<Book> matches;

    std::copy_if(books_.begin(), books_.end(), std::back_inserter(matches),
                 [&normalized_keyword](const Book& book) {
                     return to_lower(book.author).find(normalized_keyword) != std::string::npos;
                 });

    return matches;
}

std::vector<Book> LibraryService::search_by_year(int year) const {
    std::vector<Book> matches;
    std::copy_if(books_.begin(), books_.end(), std::back_inserter(matches),
                 [year](const Book& book) { return book.publication_year == year; });
    return matches;
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
    std::vector<Book> result;
    for (const auto& book : books_) {
        if (!book.borrowed) {
            result.push_back(book);
        }
    }
    return result;
}

std::vector<Book> LibraryService::search_by_author_and_year(std::string_view author,
                                                            int year) const {
    const std::string normalized_author = to_lower(author);
    std::vector<Book> matches;
    std::copy_if(books_.begin(), books_.end(), std::back_inserter(matches),
                 [&normalized_author, year](const Book& book) {
                     return to_lower(book.author).find(normalized_author) != std::string::npos &&
                            book.publication_year == year;
                 });
    return matches;
}

std::vector<Book> LibraryService::search_by_borrowed(bool borrowed) const {
    std::vector<Book> matches;
    std::copy_if(books_.begin(), books_.end(), std::back_inserter(matches),
                 [borrowed](const Book& book) { return book.borrowed == borrowed; });
    return matches;
}

std::vector<Book> LibraryService::search_by_author_and_borrowed(std::string_view author,
                                                                bool borrowed) const {
    const std::string normalized_author = to_lower(author);
    std::vector<Book> matches;
    std::copy_if(books_.begin(), books_.end(), std::back_inserter(matches),
                 [&normalized_author, borrowed](const Book& book) {
                     return to_lower(book.author).find(normalized_author) != std::string::npos &&
                            book.borrowed == borrowed;
                 });
    return matches;
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

} // namespace library
