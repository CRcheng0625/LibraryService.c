#include "library/book_repository.hpp"
#include "library/book_validation.hpp"

#include <mysqlx/xdevapi.h>

#include <unordered_set>
#include <utility>

namespace library {

MySqlBookRepository::MySqlBookRepository(MySqlConnectionConfig config)
    : config_(std::move(config)) {}

SaveStatus MySqlBookRepository::save(const std::vector<Book>& books) {
    std::unordered_set<int> seen_ids;
    for (const Book& book : books) {
        if (!is_valid_book(book) || !seen_ids.insert(book.id).second) {
            return SaveStatus::invalid_book;
        }
    }

    try {
        mysqlx::Session session(config_.host, config_.port, config_.user, config_.password,
                                config_.schema);
        session.startTransaction();
        session.sql("DELETE FROM books").execute();

        for (const Book& book : books) {
            session.sql("INSERT INTO books "
                        "(id, title, author, publication_year, borrowed) "
                        "VALUES (?, ?, ?, ?, ?)")
                .bind(book.id, book.title, book.author, book.publication_year, book.borrowed)
                .execute();
        }

        session.commit();
        return SaveStatus::success;
    } catch (const mysqlx::Error&) {
        return SaveStatus::write_error;
    }
}

LoadResult MySqlBookRepository::load() const {
    try {
        mysqlx::Session session(config_.host, config_.port, config_.user, config_.password,
                                config_.schema);
        auto result = session.sql("SELECT id, title, author, publication_year, borrowed "
                                  "FROM books ORDER BY id")
                           .execute();

        std::vector<Book> books;
        for (const mysqlx::Row& row : result) {
            Book book{row[0].get<int>(), row[1].get<std::string>(), row[2].get<std::string>(),
                      row[3].get<int>(), row[4].get<bool>()};
            if (!is_valid_book(book)) {
                return {{}, LoadStatus::invalid_format};
            }
            books.push_back(std::move(book));
        }

        return {std::move(books), LoadStatus::success};
    } catch (const mysqlx::Error&) {
        return {{}, LoadStatus::open_error};
    }
}

} // namespace library
