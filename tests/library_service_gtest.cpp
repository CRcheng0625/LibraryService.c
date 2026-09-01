#include "library/library_service.hpp"

#include <gtest/gtest.h>

TEST(LibraryServiceTest, AddsAndFindsBook) {
    library::LibraryService service;

    EXPECT_TRUE(service.add_book({1, "Clean Code", "Robert C. Martin", 2008, false}));

    const auto book = service.find_by_id(1);

    ASSERT_TRUE(book.has_value());
    EXPECT_EQ(book->title, "Clean Code");
}

TEST(LibraryServiceTest, RejectsDuplicateId) {
    library::LibraryService service;

    EXPECT_TRUE(service.add_book({1, "Clean Code", "Robert C. Martin", 2008, false}));
    EXPECT_FALSE(service.add_book({1, "Another Book", "Another Author", 2020, false}));
    EXPECT_EQ(service.all_books().size(), 1U);
}

TEST(LibraryServiceTest, RejectsInvalidBookData) {
    library::LibraryService service;

    EXPECT_FALSE(service.add_book({0, "Invalid id", "Author", 2020, false}));
    EXPECT_FALSE(service.add_book({-1, "Negative id", "Author", 2020, false}));
    EXPECT_FALSE(service.add_book({2, "", "Author", 2020, false}));
    EXPECT_TRUE(service.all_books().empty());
}

TEST(LibraryServiceTest, BorrowsAndReturnsBook) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({7, "Effective Modern C++", "Scott Meyers", 2014, false}));

    EXPECT_TRUE(service.borrow_book(7));
    EXPECT_FALSE(service.borrow_book(7));
    EXPECT_TRUE(service.return_book(7));
}

TEST(LibraryServiceTest, RejectsInvalidBorrowAndReturnOperations) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({7, "Effective Modern C++", "Scott Meyers", 2014, false}));

    EXPECT_FALSE(service.borrow_book(99));
    EXPECT_FALSE(service.return_book(7));
    EXPECT_FALSE(service.return_book(99));
}

TEST(LibraryServiceTest, UpdatesExistingBook) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({5, "Old Title", "Old Author", 2000, false}));

    ASSERT_TRUE(service.update_book(5, "New Title", "New Author", 2025));

    const auto book = service.find_by_id(5);
    ASSERT_TRUE(book.has_value());
    EXPECT_EQ(book->id, 5);
    EXPECT_EQ(book->title, "New Title");
    EXPECT_EQ(book->author, "New Author");
    EXPECT_EQ(book->publication_year, 2025);
}

TEST(LibraryServiceTest, RejectsUpdateForMissingBook) {
    library::LibraryService service;

    EXPECT_FALSE(service.update_book(99, "New Title", "New Author", 2025));
}

TEST(LibraryServiceTest, RejectsUpdateWithEmptyText) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({5, "Old Title", "Old Author", 2000, false}));

    EXPECT_FALSE(service.update_book(5, "", "New Author", 2025));
    EXPECT_FALSE(service.update_book(5, "New Title", "", 2025));

    const auto book = service.find_by_id(5);
    ASSERT_TRUE(book.has_value());
    EXPECT_EQ(book->title, "Old Title");
    EXPECT_EQ(book->author, "Old Author");
}

TEST(LibraryServiceTest, UpdatePreservesBorrowedStatus) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({5, "Old Title", "Old Author", 2000, true}));

    ASSERT_TRUE(service.update_book(5, "New Title", "New Author", 2025));

    const auto book = service.find_by_id(5);
    ASSERT_TRUE(book.has_value());
    EXPECT_TRUE(book->borrowed);
}

TEST(LibraryServiceTest, SearchesCaseInsensitively) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({1, "Algorithms", "Robert Sedgewick", 2011, false}));
    ASSERT_TRUE(service.add_book({2, "Introduction to Algorithms", "Thomas Cormen", 2009, false}));

    const auto matches = service.search_by_title("ALGORITHMS");

    ASSERT_EQ(matches.size(), 2U);
    EXPECT_EQ(matches[0].id, 1);
    EXPECT_EQ(matches[1].id, 2);
}

TEST(LibraryServiceTest, ReturnsNoResultsForUnknownSearch) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({1, "Algorithms", "Robert Sedgewick", 2011, false}));

    EXPECT_TRUE(service.search_by_title("Python").empty());
    EXPECT_TRUE(service.search_by_author("Unknown").empty());
    EXPECT_TRUE(service.search_by_year(1990).empty());
}

TEST(LibraryServiceTest, SortsBooksByTitleAndId) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({3, "C++", "Author C", 2022, false}));
    ASSERT_TRUE(service.add_book({2, "Algorithms", "Author B", 2021, false}));
    ASSERT_TRUE(service.add_book({1, "C++", "Author A", 2020, false}));

    const auto sorted = service.books_sorted_by_title();

    ASSERT_EQ(sorted.size(), 3U);
    EXPECT_EQ(sorted[0].title, "Algorithms");
    EXPECT_EQ(sorted[1].id, 1);
    EXPECT_EQ(sorted[2].id, 3);
}

TEST(LibraryServiceTest, TitleSortingDoesNotChangeOriginalOrder) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({3, "C++", "Author C", 2022, false}));
    ASSERT_TRUE(service.add_book({2, "Algorithms", "Author B", 2021, false}));

    const auto sorted = service.books_sorted_by_title();

    ASSERT_EQ(sorted.size(), 2U);
    ASSERT_EQ(service.all_books().size(), 2U);
    EXPECT_EQ(service.all_books()[0].id, 3);
    EXPECT_EQ(service.all_books()[1].id, 2);
}

TEST(LibraryServiceTest, SortingEmptyLibraryReturnsEmptyVector) {
    library::LibraryService service;

    EXPECT_TRUE(service.books_sorted_by_title().empty());
}

TEST(LibraryServiceTest, ReturnsRequestedBookPage) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({1, "First", "Author A", 2020, false}));
    ASSERT_TRUE(service.add_book({2, "Second", "Author B", 2021, false}));
    ASSERT_TRUE(service.add_book({3, "Third", "Author C", 2022, false}));

    const auto page = service.books_page(0, 2);

    ASSERT_EQ(page.books.size(), 2U);
    EXPECT_EQ(page.books[0].id, 1);
    EXPECT_EQ(page.books[1].id, 2);
    EXPECT_EQ(page.total_books, 3U);
    EXPECT_TRUE(page.has_next);
}

TEST(LibraryServiceTest, HandlesBookPageBoundaries) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({1, "First", "Author A", 2020, false}));
    ASSERT_TRUE(service.add_book({2, "Second", "Author B", 2021, false}));
    ASSERT_TRUE(service.add_book({3, "Third", "Author C", 2022, false}));

    const auto out_of_range_page = service.books_page(3, 1);
    EXPECT_TRUE(out_of_range_page.books.empty());
    EXPECT_EQ(out_of_range_page.total_books, 3U);
    EXPECT_FALSE(out_of_range_page.has_next);

    const auto zero_limit_page = service.books_page(0, 0);
    EXPECT_TRUE(zero_limit_page.books.empty());
    EXPECT_EQ(zero_limit_page.total_books, 3U);
    EXPECT_FALSE(zero_limit_page.has_next);

    const auto final_page = service.books_page(2, 10);
    ASSERT_EQ(final_page.books.size(), 1U);
    EXPECT_EQ(final_page.books.front().id, 3);
    EXPECT_EQ(final_page.total_books, 3U);
    EXPECT_FALSE(final_page.has_next);
}

TEST(LibraryServiceTest, RebuildsIndexAfterMiddleRemoval) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book({1, "First", "Author A", 2020, false}));
    ASSERT_TRUE(service.add_book({2, "Second", "Author B", 2021, false}));
    ASSERT_TRUE(service.add_book({3, "Third", "Author C", 2022, false}));

    ASSERT_TRUE(service.remove_book(2));

    const auto third = service.find_by_id(3);
    ASSERT_TRUE(third.has_value());
    EXPECT_EQ(third->title, "Third");
    EXPECT_TRUE(service.borrow_book(3));
}
