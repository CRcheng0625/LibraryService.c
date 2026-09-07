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

} // namespace library
