#include "library_io.hpp"

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

namespace {

const char* const test_file_path = "library_io_gtest_books.txt";

void write_test_file() {
    std::ofstream output(test_file_path);
    ASSERT_TRUE(output.is_open());
    output << "1 \"Test Book\" \"Test Author\" 2024 0\n";
}

} // namespace

TEST(LibraryIoTest, LoadsBooksAndShowsStatusByDefault) {
    write_test_file();

    library::LibraryService service;
    testing::internal::CaptureStdout();
    const bool success = library_io::load_library(service, test_file_path);
    const std::string output = testing::internal::GetCapturedStdout();

    std::remove(test_file_path);

    ASSERT_TRUE(success);
    ASSERT_EQ(service.all_books().size(), 1U);
    EXPECT_NE(output.find("Loaded 1 book(s)."), std::string::npos);
}

TEST(LibraryIoTest, LoadsBooksWithoutShowingStatusWhenRequested) {
    write_test_file();

    library::LibraryService service;
    testing::internal::CaptureStdout();
    const bool success = library_io::load_library(service, test_file_path, false);
    const std::string output = testing::internal::GetCapturedStdout();

    std::remove(test_file_path);

    ASSERT_TRUE(success);
    ASSERT_EQ(service.all_books().size(), 1U);
    EXPECT_TRUE(output.empty());
}

