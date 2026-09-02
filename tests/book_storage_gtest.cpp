#include "library/book_storage.hpp"

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {

void remove_file(const std::string& file_path) {
    std::remove(file_path.c_str());
}

}  // namespace

TEST(BookStorageTest, SavesAndLoadsBooks) {
    const std::string file_path = "storage_gtest_books.txt";
    const std::vector<library::Book> books{
        {1, "C++ Primer", "Stanley Lippman", 2012, false},
        {2, "Clean Code", "Robert C. Martin", 2008, true}};

    ASSERT_TRUE(library::save_books_to_file(books, file_path));

    const auto result = library::load_books_from_file(file_path);
    remove_file(file_path);

    ASSERT_EQ(result.status, library::LoadStatus::success);
    ASSERT_EQ(result.books.size(), 2U);
    EXPECT_EQ(result.books[0].title, "C++ Primer");
    EXPECT_EQ(result.books[1].author, "Robert C. Martin");
    EXPECT_TRUE(result.books[1].borrowed);
}

TEST(BookStorageTest, ReportsMissingFile) {
    const auto result =
        library::load_books_from_file("storage_gtest_missing.txt");

    EXPECT_EQ(result.status, library::LoadStatus::file_not_found);
    EXPECT_TRUE(result.books.empty());
}

TEST(BookStorageTest, ReportsInvalidFormat) {
    const std::string file_path = "storage_gtest_invalid.txt";
    {
        std::ofstream output(file_path);
        ASSERT_TRUE(output.is_open());
        output << "not a valid book record\n";
    }

    const auto result = library::load_books_from_file(file_path);
    remove_file(file_path);

    EXPECT_EQ(result.status, library::LoadStatus::invalid_format);
    EXPECT_TRUE(result.books.empty());
}

TEST(BookStorageTest, EmptyFileIsSuccessful) {
    const std::string file_path = "storage_gtest_empty.txt";
    {
        std::ofstream output(file_path);
        ASSERT_TRUE(output.is_open());
    }

    const auto result = library::load_books_from_file(file_path);
    remove_file(file_path);

    EXPECT_EQ(result.status, library::LoadStatus::success);
    EXPECT_TRUE(result.books.empty());
}

TEST(BookStorageTest, RejectsDuplicateIds) {
    const std::string file_path = "storage_gtest_duplicate_ids.txt";
    {
        std::ofstream output(file_path);
        ASSERT_TRUE(output.is_open());
        output << "1 \"First\" \"Author A\" 2020 0\n";
        output << "1 \"Second\" \"Author B\" 2021 0\n";
    }

    const auto result = library::load_books_from_file(file_path);
    remove_file(file_path);

    EXPECT_EQ(result.status, library::LoadStatus::invalid_format);
    EXPECT_TRUE(result.books.empty());
}

TEST(BookStorageTest, RejectsNonPositiveIds) {
    const std::string file_path = "storage_gtest_invalid_id.txt";
    {
        std::ofstream output(file_path);
        ASSERT_TRUE(output.is_open());
        output << "0 \"Book\" \"Author\" 2020 0\n";
    }

    const auto result = library::load_books_from_file(file_path);
    remove_file(file_path);

    EXPECT_EQ(result.status, library::LoadStatus::invalid_format);
    EXPECT_TRUE(result.books.empty());
}

TEST(BookStorageTest, RejectsEmptyTitle) {
    const std::string file_path = "storage_gtest_empty_title.txt";
    {
        std::ofstream output(file_path);
        ASSERT_TRUE(output.is_open());
        output << "1 \"\" \"Author\" 2020 0\n";
    }

    const auto result = library::load_books_from_file(file_path);
    remove_file(file_path);

    EXPECT_EQ(result.status, library::LoadStatus::invalid_format);
    EXPECT_TRUE(result.books.empty());
}

TEST(BookStorageTest, RejectsEmptyAuthor) {
    const std::string file_path = "storage_gtest_empty_author.txt";
    {
        std::ofstream output(file_path);
        ASSERT_TRUE(output.is_open());
        output << "1 \"Book\" \"\" 2020 0\n";
    }

    const auto result = library::load_books_from_file(file_path);
    remove_file(file_path);

    EXPECT_EQ(result.status, library::LoadStatus::invalid_format);
    EXPECT_TRUE(result.books.empty());
}

TEST(BookStorageTest, RejectsInvalidBookWhenSaving) {
    const std::string file_path = "storage_gtest_invalid_save.txt";

    const std::vector<library::Book> books{{1, "", "Author", 2020, false}};

    EXPECT_FALSE(library::save_books_to_file(books, file_path));

    remove_file(file_path);
}

TEST(BookStorageTest, ReportsSaveFailureForInvalidPath) {
    const std::vector<library::Book> books{{1, "Book", "Author", 2020, false}};

    EXPECT_FALSE(library::save_books_to_file(books, "directory_that_does_not_exist/books.txt"));
}

TEST(BookStorageTest, RejectsDuplicateIdsWhenSaving) {
    const std::string file_path = "storage_gtest_duplicate_save.txt";

    const std::vector<library::Book> books{{1, "First", "Author A", 2020, false},
                                           {1, "Second", "Author B", 2021, false}};

    EXPECT_FALSE(library::save_books_to_file(books, file_path));

    remove_file(file_path);
}