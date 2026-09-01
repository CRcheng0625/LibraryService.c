#pragma once

#include "library/book.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace library {

struct BookPage {
    std::vector<Book> books;
    std::size_t total_books{};
    bool has_next{};
};

struct BookFilter {
    std::optional<std::string> title_keyword;
    std::optional<std::string> author_keyword;
    std::optional<int> publication_year;
    std::optional<bool> borrowed;
};

enum class LoanResult {
    success,
    book_not_found,
    already_borrowed,
    not_borrowed
};

class LibraryService {
  public:
    bool add_book(Book book);
    bool remove_book(int id);
    LoanResult borrow_book(int id);
    LoanResult return_book(int id);
    bool update_book(int id, std::string title, std::string author, int publication_year);

    [[nodiscard]] std::optional<Book> find_by_id(int id) const;
    [[nodiscard]] std::vector<Book> search_by_title(std::string_view keyword) const;
    [[nodiscard]] std::vector<Book> search_by_author(std::string_view keyword) const;
    [[nodiscard]] std::vector<Book> search_by_year(int year) const;
    [[nodiscard]] std::vector<Book> search_by_author_and_year(std::string_view author, int year) const;
    [[nodiscard]] std::vector<Book> search_by_borrowed(bool borrowed) const;
    [[nodiscard]] std::vector<Book> search_by_author_and_borrowed(std::string_view author, bool borrowed) const;
    [[nodiscard]] std::vector<Book> books_sorted_by_title() const;
    [[nodiscard]] std::vector<Book> books_sorted_by_year() const;
    [[nodiscard]] std::vector<Book> available_books() const;
    [[nodiscard]] BookPage books_page(std::size_t offset, std::size_t limit) const;
    [[nodiscard]] const std::vector<Book>& all_books() const noexcept;
    [[nodiscard]] std::vector<Book> filter_books(const BookFilter& filter) const;

  private:
    void rebuild_index();

    std::vector<Book> books_;
    std::unordered_map<int, std::size_t> id_index_; // id / books_[index]
};

} // namespace library
