#pragma once

#include "library/book_storage.hpp"

#include <string>
#include <vector>

namespace library {

// A repository hides where books are stored from the rest of the application.
class BookRepository {
  public:
    virtual ~BookRepository() = default;

    [[nodiscard]] virtual SaveStatus save(const std::vector<Book>& books) = 0;
    [[nodiscard]] virtual LoadResult load() const = 0;
};

class FileBookRepository final : public BookRepository {
  public:
    explicit FileBookRepository(std::string file_path);

    [[nodiscard]] SaveStatus save(const std::vector<Book>& books) override;
    [[nodiscard]] LoadResult load() const override;

  private:
    std::string file_path_;
};

struct MySqlConnectionConfig {
    std::string host{"127.0.0.1"};
    unsigned port{33060};
    std::string user{"root"};
    std::string password;
    std::string schema{"library_app"};
};

class MySqlBookRepository final : public BookRepository {
  public:
    explicit MySqlBookRepository(MySqlConnectionConfig config);

    [[nodiscard]] SaveStatus save(const std::vector<Book>& books) override;
    [[nodiscard]] LoadResult load() const override;

  private:
    MySqlConnectionConfig config_;
};

} // namespace library
