#include "library/book_validation.hpp"

#include <gtest/gtest.h>

TEST(BookValidationTest, AcceptsValidBook) {
    const library::Book book{1, "C++ Primer", "Stanley Lippman", 2012, false};

    EXPECT_TRUE(library::is_valid_book(book));
}

TEST(BookValidationTest, RejectsInvalidBookFields) {
    EXPECT_FALSE(library::is_valid_book({0, "Book", "Author", 2020, false}));

    EXPECT_FALSE(library::is_valid_book({1, "", "Author", 2020, false}));

    EXPECT_FALSE(library::is_valid_book({1, "Book", "", 2020, false}));
}