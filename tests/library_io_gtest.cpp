#include "library_io.hpp"

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <vector>

namespace {

const char* const test_file_path = "library_io_gtest_books.txt";

class MemoryBookRepository final : public library::BookRepository {
  public:
    explicit MemoryBookRepository(std::vector<library::Book> books) : books_(std::move(books)) {}

    library::SaveStatus save(const std::vector<library::Book>& books) override {
        books_ = books;
        return library::SaveStatus::success;
    }

    library::LoadResult load() const override {
        return {books_, library::LoadStatus::success};
    }

  private:
    std::vector<library::Book> books_;
};

void write_test_file() {
    std::ofstream output(test_file_path);
    ASSERT_TRUE(output.is_open());
    output << "1 \"Test Book\" \"Test Author\" 2024 0\n";
}

} // namespace

TEST(LibraryIoTest, LoadsBooksThroughRepositoryInterface) {
    MemoryBookRepository repository({{1, "Memory Book", "Test Author", 2024, false}});
    library::LibraryService service;

    testing::internal::CaptureStdout();
    const bool success = library_io::load_library(service, repository, false);
    const std::string output = testing::internal::GetCapturedStdout();

    ASSERT_TRUE(success);
    ASSERT_EQ(service.all_books().size(), 1U);
    EXPECT_EQ(service.all_books().front().title, "Memory Book");
    EXPECT_TRUE(output.empty());
}

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

TEST(LibraryIoTest, ReportsInvalidFileWhenChecking) {
    const std::string file_path = "library_io_gtest_invalid.txt";
    {
        std::ofstream output(file_path);
        ASSERT_TRUE(output.is_open());
        output << "not a valid book record\n";
    }

    testing::internal::CaptureStderr();
    const bool success = library_io::check_library_file(file_path);
    const std::string error = testing::internal::GetCapturedStderr();

    std::remove(file_path.c_str());

    EXPECT_FALSE(success);
    EXPECT_NE(error.find("Books file has invalid format."), std::string::npos);
}

TEST(LibraryIoTest, ReportsMissingFileWhenChecking) {
    const std::string file_path = "library_io_gtest_missing.txt";
    std::remove(file_path.c_str());

    testing::internal::CaptureStderr();
    const bool success = library_io::check_library_file(file_path);
    const std::string error = testing::internal::GetCapturedStderr();

    EXPECT_FALSE(success);
    EXPECT_NE(error.find("Books file was not found."), std::string::npos);
}
