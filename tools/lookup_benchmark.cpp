#include "library/library_service.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <optional>

namespace {

std::optional<library::Book> linear_find_by_id(
    const std::vector<library::Book>& books,
    int id) {
    const auto it = std::find_if(
        books.begin(), books.end(),
        [id](const library::Book& book) {
            return book.id == id;
        });

    if (it == books.end()) {
        return std::nullopt;
    }

    return *it;
}

}  // namespace

int main() {
    library::LibraryService service;

    constexpr int book_count = 2000;
    constexpr int repetitions = 100000;

    for (int id = 1; id <= book_count; ++id) {
        service.add_book({id, "Benchmark book", "Benchmark author", 2025, false});
    }

    std::size_t linear_hits = 0;
    const auto linear_start = std::chrono::steady_clock::now();
    for (int repetition = 0; repetition < repetitions; ++repetition) {
        linear_hits += linear_find_by_id(service.all_books(), book_count).has_value();
    }
    const auto linear_end = std::chrono::steady_clock::now();


    std::size_t indexed_hits = 0;
    const auto indexed_start = std::chrono::steady_clock::now();
    for (int repetition = 0; repetition < repetitions; ++repetition) {
        indexed_hits += service.find_by_id(book_count).has_value();
    }
    const auto indexed_end = std::chrono::steady_clock::now();

    const auto linear_elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(
            linear_end - linear_start);
    const auto indexed_elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(
            indexed_end - indexed_start);

    std::cout << "Linear: " << linear_elapsed.count() << " microseconds, hits = "
              << linear_hits << '\n';
    std::cout << "Indexed: " << indexed_elapsed.count() << " microseconds, hits = "
              << indexed_hits << '\n';

    return 0;
}
