#include "library_io.hpp"

#include "library/book_repository.hpp"

#include <iostream>

namespace library_io {

bool save_library(const library::LibraryService& service, const std::string& file_path) {
    library::FileBookRepository repository(file_path);
    const auto status = repository.save(service.all_books());

    switch (status) {
    case library::SaveStatus::success:
        return true;

    case library::SaveStatus::invalid_book:
        std::cerr << "Cannot save invalid book data.\n";
        break;

    case library::SaveStatus::open_error:
        std::cerr << "Failed to open books file for saving.\n";
        break;

    case library::SaveStatus::write_error:
        std::cerr << "Failed while writing books file.\n";
        break;
    }

    return false;
}

bool load_library(library::LibraryService& service, const std::string& file_path, bool show_status) {
    const library::FileBookRepository repository(file_path);
    const auto result = repository.load();

    switch (result.status) {
    case library::LoadStatus::success:
        for (const auto& book : result.books) {
            service.add_book(book);
        }
        if (show_status) {
            std::cout << "Loaded " << result.books.size() << " book(s).\n";
        }
        return true;

    case library::LoadStatus::file_not_found:
        if (show_status) {
            std::cout << "Books file not found. "
                         "Starting with an empty library.\n";
        }
        return true;

    case library::LoadStatus::open_error:
        std::cerr << "Failed to open books file.\n";
        return false;

    case library::LoadStatus::invalid_format:
        std::cerr << "Books file has invalid format.\n";
        return false;
    }

    return false;
}

bool check_library_file(const std::string& file_path) {
    const library::FileBookRepository repository(file_path);
    const auto result = repository.load();

    switch (result.status) {
    case library::LoadStatus::success:
        std::cout << "Books file is valid. Loaded " << result.books.size() << " book(s).\n";
        return true;

    case library::LoadStatus::file_not_found:
        std::cerr << "Books file was not found.\n";
        return false;

    case library::LoadStatus::open_error:
        std::cerr << "Books file could not be opened.\n";
        return false;

    case library::LoadStatus::invalid_format:
        std::cerr << "Books file has invalid format.\n";
        return false;
    }

    return false;
}

} // namespace library_io
