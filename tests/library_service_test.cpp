#include "library/library_service.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

int failures = 0;

void expect(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}

void test_add_and_find_book() {
    library::LibraryService service;
    expect(service.add_book({1, "Clean Code", "Robert C. Martin", 2008, false}),
           "a valid book should be added");

    expect(service.all_books().size() == 1,
        "adding one book should increase the book count to one");

    expect(!service.add_book({1, "Duplicate", "Someone", 2020, false}),
           "duplicate ids should be rejected");

    expect(service.all_books().size() == 1,
        "rejecting a duplicate id should not change the book count");

    const auto book = service.find_by_id(1);
    expect(book.has_value(), "an existing book should be found");
    expect(book && book->title == "Clean Code", "the found book should have the expected title");
}

void test_borrow_and_return_book() {
    library::LibraryService service;
    service.add_book({7, "Effective Modern C++", "Scott Meyers", 2014, false});

    expect(service.borrow_book(7), "an available book should be borrowable");
    expect(!service.borrow_book(7), "an already borrowed book should not be borrowed twice");
    expect(service.return_book(7), "a borrowed book should be returnable");
}

void test_search_and_sort() {
    library::LibraryService service;
    service.add_book({1, "Algorithms", "Robert Sedgewick", 2011, false});
    service.add_book({2, "C++ Primer", "Stanley Lippman", 2012, false});
    service.add_book({3, "Introduction to Algorithms", "Thomas Cormen", 2009, false});

    expect(service.search_by_title("Python").empty(),
           "a missing title should produce no matches");

    expect(service.search_by_title("ALGORITHMS").size() == 2,
           "title search should be case insensitive");

    const auto sorted = service.books_sorted_by_year();
    expect(sorted.size() == 3 && sorted.front().publication_year == 2009,
           "books should be sorted by publication year");

    const auto author_matches =
        service.search_by_author("cormen");

    expect(author_matches.size() == 1 &&
        author_matches.front().id == 3,
        "author search should find the matching book");

    expect(service.search_by_author("LIPPMAN").size() == 1,
        "author search should be case insensitive");

	const auto year_matches = service.search_by_year(2012);

    expect(year_matches.size() == 1 && year_matches.front().id == 2,
        "year search should find the matching books");

    expect(service.search_by_year(1990).empty(),
		"year search should return no matches for an unknown year");


    expect(service.borrow_book(2),
           "a test book should be borrowable");

    const auto borrowed_matches =
        service.search_by_borrowed(true);

    expect(borrowed_matches.size() == 1 &&
        borrowed_matches.front().id == 2,
        "borrowed search should find borrowed books");

    const auto available_matches =
        service.search_by_borrowed(false);

    expect(available_matches.size() == 2,
        "borrowed search should find available books");

    const auto combined_matches =
        service.search_by_author_and_year("LIPPMAN", 2012);

    expect(combined_matches.size() == 1 &&
               combined_matches.front().id == 2,
           "combined search should match author and year");

    expect(service.search_by_author_and_year("LIPPMAN", 1990).empty(),
           "combined search should require both author and year");
}

void test_remove_book() {
    library::LibraryService service;
    service.add_book({10, "The Book", "Test Author", 2024, false});

    expect(service.remove_book(10),
           "an existing book should be removed");
    expect(!service.find_by_id(10).has_value(),
           "a removed book should no longer be found");
    expect(!service.remove_book(99),
           "removing a missing book should fail");
}

void test_available_books() {
    library::LibraryService service;
    service.add_book({ 1,"Available","Author",2020,false });
    service.add_book({ 2,"Borrowed","Author",2021,false });
    service.borrow_book(2);

    const auto books = service.available_books();

    expect(books.size() == 1,
        "only available books should be returned");
    expect(books.front().id == 1,
        "the available book should be returned");
}

void test_borrow_missing_book() {
    // Arrange
    library::LibraryService service;
    service.add_book({ 1, "C++ Primer", "Stanley Lippman", 2012, false });

    // Act
    const bool success = service.borrow_book(99);

    // Assert
    expect(!success,
        "borrowing a missing book should fail");

    const auto existing_book = service.find_by_id(1);
    expect(existing_book && !existing_book->borrowed,
        "borrowing a missing id should not affect existing books");
}

void test_find_missing_book() {
    library::LibraryService service;
    service.add_book({1, "C++ Primer", "Stanley Lippman", 2012, false});

    const auto missing_book = service.find_by_id(99);

    expect(!missing_book,
        "a missing id should return an empty optional");

    const auto book = service.find_by_id(1);

    expect(book.has_value(),
           "an existing book should be found");

    if (book) {
        expect(book->title == "C++ Primer",
            "the found book should have the expected title");
    }
}

void test_index_after_middle_removal() {
    library::LibraryService service;
    service.add_book({1, "First", "Author A", 2020, false});
    service.add_book({2, "Second", "Author B", 2021, false});
    service.add_book({3, "Third", "Author C", 2022, false});

    expect(service.remove_book(2), "the middle book should be removed");

    const auto first = service.find_by_id(1);
    const auto third = service.find_by_id(3);
    const auto removed = service.find_by_id(2);

    expect(first && first->title == "First", "the first book should remain searchable");
    expect(third && third->title == "Third", 
        "the book after the removed item should remain searchable");
    expect(!removed, "the removed book should not be found");
}

}  // namespace

int main() {
    test_add_and_find_book();
    test_borrow_and_return_book();
    test_search_and_sort();
    test_remove_book();
    test_available_books();
    test_borrow_missing_book();
    test_find_missing_book();
    test_index_after_middle_removal();

    if (failures == 0) {
        std::cout << "All tests passed.\n";
        return EXIT_SUCCESS;
    }

    std::cerr << failures << " test(s) failed.\n";
    return EXIT_FAILURE;
}
