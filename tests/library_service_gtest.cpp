#include "library/library_service.hpp"

#include <gtest/gtest.h>

TEST(LibraryServiceTest, AddsAndFindsBook) {
    library::LibraryService service;

    EXPECT_TRUE(service.add_book(
        {1, "Clean Code", "Robert C. Martin", 2008, false}));

    const auto book = service.find_by_id(1);

    ASSERT_TRUE(book.has_value());
    EXPECT_EQ(book->title, "Clean Code");
}

TEST(LibraryServiceTest, RejectsDuplicateId) {
    library::LibraryService service;

    EXPECT_TRUE(service.add_book(
        {1, "Clean Code", "Robert C. Martin", 2008, false}));
    EXPECT_FALSE(service.add_book(
        {1, "Another Book", "Another Author", 2020, false}));
    EXPECT_EQ(service.all_books().size(), 1U);
}

TEST(LibraryServiceTest, RejectsInvalidBookData) {
    library::LibraryService service;

    EXPECT_FALSE(service.add_book(
        {0, "Invalid id", "Author", 2020, false}));
    EXPECT_FALSE(service.add_book(
        {-1, "Negative id", "Author", 2020, false}));
    EXPECT_FALSE(service.add_book(
        {2, "", "Author", 2020, false}));
    EXPECT_TRUE(service.all_books().empty());
}

TEST(LibraryServiceTest, BorrowsAndReturnsBook) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book(
        {7, "Effective Modern C++", "Scott Meyers", 2014, false}));

    EXPECT_TRUE(service.borrow_book(7));
    EXPECT_FALSE(service.borrow_book(7));
    EXPECT_TRUE(service.return_book(7));
}

TEST(LibraryServiceTest, RejectsInvalidBorrowAndReturnOperations) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book(
        {7, "Effective Modern C++", "Scott Meyers", 2014, false}));

    EXPECT_FALSE(service.borrow_book(99));
    EXPECT_FALSE(service.return_book(7));
    EXPECT_FALSE(service.return_book(99));
}

TEST(LibraryServiceTest, SearchesCaseInsensitively) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book(
        {1, "Algorithms", "Robert Sedgewick", 2011, false}));
    ASSERT_TRUE(service.add_book(
        {2, "Introduction to Algorithms", "Thomas Cormen", 2009, false}));

    const auto matches = service.search_by_title("ALGORITHMS");

    ASSERT_EQ(matches.size(), 2U);
    EXPECT_EQ(matches[0].id, 1);
    EXPECT_EQ(matches[1].id, 2);
}

TEST(LibraryServiceTest, ReturnsNoResultsForUnknownSearch) {
    library::LibraryService service;
    ASSERT_TRUE(service.add_book(
        {1, "Algorithms", "Robert Sedgewick", 2011, false}));

    EXPECT_TRUE(service.search_by_title("Python").empty());
    EXPECT_TRUE(service.search_by_author("Unknown").empty());
    EXPECT_TRUE(service.search_by_year(1990).empty());
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
