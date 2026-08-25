#include "library/book_storage.hpp"
#include "library/library_service.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

namespace {

int failures = 0;

void expect(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}

void test_save_books() {
    library::LibraryService service;
    service.add_book({1, "C++ Primer", "Stanley Lippman", 2012, false});

    const std::string file_path = "test_books.txt";

    const bool success = library::save_books_to_file(service.all_books(), file_path);

    expect(success, "saving books should succeed");

    std::remove(file_path.c_str());
}

void test_save_and_load_books() {
    library::LibraryService service;
    service.add_book({1, "C++ Primer", "Stanley Lippman", 2012, false});
    service.add_book({2, "Clean Code", "Robert C. Martin", 2008, true});

    const std::string file_path = "test_books.txt";

    const bool save_success =
        library::save_books_to_file(service.all_books(), file_path);

    expect(save_success, "saving books should succeed");

    if (!save_success) {
        std::remove(file_path.c_str());
        return;
    }

    const auto loaded_books = library::load_books_from_file(file_path);

    if (loaded_books.size() != 2) {
        expect(false, "loading should restore two books");
        std::remove(file_path.c_str());
        return;
    }

    expect(loaded_books[0].title == "C++ Primer" && loaded_books[1].borrowed,
           "loading should restore book data");

    std::remove(file_path.c_str());
}

void test_load_missing_file() {
    const auto books = library::load_books_from_file("file_that_does_not_exist.txt");

    expect(books.empty(), "loading a missing file should return an empty vector");
}

void test_load_invalid_file() {
    const std::string file_path = "invalid_books.txt";

    {
        std::ofstream output(file_path);
        output << "this is not a valid book record\n";
    }

    const auto books = library::load_books_from_file(file_path);

    expect(books.empty(), "an invalid file should return an empty vector");

    std::remove(file_path.c_str());
}

} //namespace

int main() {
    test_save_books();
    test_save_and_load_books();
    test_load_missing_file();
    test_load_invalid_file();

    if (failures == 0) {
        std::cout << "All storage tests passed.\n";
        return EXIT_SUCCESS;
    }

    std::cerr << failures << " storage test(s) failed.\n";
    return EXIT_FAILURE;
}
