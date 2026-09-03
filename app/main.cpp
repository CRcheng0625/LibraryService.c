#include "library/book_storage.hpp"
#include "library/library_service.hpp"

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <utility>

namespace {

enum class MenuChoice : unsigned char {
    exit = 0,
    list_books = 1,
    add_book = 2,
    borrow_book = 3,
    return_book = 4,
    remove_book = 5,
    search_title = 6,
    find_by_id = 7,
    list_by_year = 8,
    list_available = 9,
    search_author = 10,
    search_year = 11,
    search_author_and_year = 12,
    search_borrowed = 13,
    search_author_and_borrowed = 14,
    update_book = 15,
    list_by_title = 16,
    list_page = 17,
    filter_books = 18,
    show_statistics = 19,
    save_now = 20
};

enum class MenuAction : unsigned char {
    continue_running,
    exit_success,
    exit_failure
};

bool read_int(int& value) {
    if (std::cin >> value) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return true;
    }

    if (std::cin.eof()) {
        return false;
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Please enter a number.\n";
    return false;
}

std::string read_line(const std::string& prompt) {
    std::cout << prompt;

    std::string line;
    std::getline(std::cin, line);
    return line;
}

void print_menu() {
    std::cout << "\nLibrary manager\n"
              << "1. List books\n"
              << "2. Add book\n"
              << "3. Borrow book\n"
              << "4. Return book\n"
              << "5. Remove book\n"
              << "6. Search by title\n"
              << "7. Find book by id\n"
              << "8. List books by year\n"
              << "9. List available books\n"
              << "10. Search by author\n"
              << "11. Search by year\n"
              << "12. Search by author and year\n"
              << "13. Search by borrowed\n"
              << "14. Search by author and borrowed status\n"
              << "15. Update book\n"
              << "16. List books by title\n"
              << "17. List books by page\n"
              << "18. Filter books\n"
              << "19. Show statistics\n"
              << "20. Save now\n"
              << "0. Exit\n"
              << "Choose: ";
}

void print_book(const library::Book& book) {
    std::cout << book.id << " | " << book.title << " | " << book.author << " | "
              << book.publication_year << " | " << (book.borrowed ? "borrowed" : "available")
              << '\n';
}

void list_books(const library::LibraryService& service) {
    if (service.all_books().empty()) {
        std::cout << "No books yet.\n";
        return;
    }

    for (const auto& book : service.all_books()) {
        print_book(book);
    }
}

void add_book(library::LibraryService& service) {
    library::Book book;
    std::cout << "Id: ";
    if (!read_int(book.id)) {
        return;
    }

    book.title = read_line("Title: ");
    if (book.title.empty()) {
        std::cout << "Title cannot be empty.\n";
        return;
    }

    book.author = read_line("Author: ");
    if (book.author.empty()) {
        std::cout << "Author cannot be empty.\n";
        return;
    }
    std::cout << "Publication year: ";

    if (!read_int(book.publication_year)) {
        return;
    }

    std::cout << (service.add_book(std::move(book)) ? "Book added.\n"
                                                    : "Invalid or duplicate book.\n");
}

void update_book_in_library(library::LibraryService& service) {
    int id{};
    std::cout << "Book id: ";
    if (!read_int(id)) {
        return;
    }

    const auto existing_book = service.find_by_id(id);
    if (!existing_book) {
        std::cout << "Book not found.\n";
        return;
    }

    std::cout << "Current book: ";
    print_book(*existing_book);

    const std::string title = read_line("New title: ");
    const std::string author = read_line("New author: ");

    int publication_year{};
    std::cout << "New publication year: ";
    if (!read_int(publication_year)) {
        return;
    }

    const bool success = service.update_book(id, title, author, publication_year);
    std::cout << (success ? "Book updated.\n" : "Invalid book data.\n");
}

void update_borrow_status(library::LibraryService& service, bool borrow) {
    int id{};
    std::cout << "Book id: ";
    if (!read_int(id)) {
        return;
    }
    library::LoanResult result{};
    if (borrow) {
        result = service.borrow_book(id);
    } else {
        result = service.return_book(id);
    }

    switch (result) {
    case library::LoanResult::success:
        std::cout << "Done.\n";
        break;
    case library::LoanResult::book_not_found:
        std::cout << "Book not found.\n";
        break;
    case library::LoanResult::already_borrowed:
        std::cout << "Book is already borrowed.\n";
        break;
    case library::LoanResult::not_borrowed:
        std::cout << "Book is not currently borrowed.\n";
        break;
    }
}

void remove_book_from_library(library::LibraryService& service) {
    int id{};
    std::cout << "Book id: ";
    if (!read_int(id)) {
        return;
    }
    const bool success = service.remove_book(id);
    std::cout << (success ? "Book removed.\n" : "Book not found.\n");
}

void search_books_by_title(const library::LibraryService& service) {
    std::string keyword = read_line("Title keyword: ");

    const auto matches = service.search_by_title(keyword);

    std::cout << "Matches: " << matches.size() << "\n";

    for (const auto& book : matches) {
        print_book(book);
    }
}

void find_book_by_id(const library::LibraryService& service) {
    int id{};
    std::cout << "Book id: ";

    if (!read_int(id)) {
        return;
    }

    const auto book = service.find_by_id(id);
    if (!book) {
        std::cout << "Book not found.\n";
        return;
    }

    print_book(*book);
}

void list_books_by_year(const library::LibraryService& service) {
    const auto books = service.books_sorted_by_year();

    if (books.empty()) {
        std::cout << "No books yet.\n";
        return;
    }

    for (const auto& book : books) {
        print_book(book);
    }
}

void list_books_by_title(const library::LibraryService& service) {
    const auto books = service.books_sorted_by_title();

    if (books.empty()) {
        std::cout << "No books yet.\n";
        return;
    }

    for (const auto& book : books) {
        print_book(book);
    }
}

bool read_non_negative_size(const std::string& prompt, std::size_t& value) {
    int input{};
    std::cout << prompt;

    if (!read_int(input) || input < 0) {
        std::cout << "Please enter a non-negative number.\n";
        return false;
    }

    value = static_cast<std::size_t>(input);
    return true;
}

void list_books_page(const library::LibraryService& service) {
    std::size_t offset{};
    std::size_t limit{};

    if (!read_non_negative_size("Offset (0-based): ", offset) ||
        !read_non_negative_size("Number of books: ", limit)) {
        return;
    }

    if (limit == 0) {
        std::cout << "Number of books must be greater than zero.\n";
        return;
    }

    const auto page = service.books_page(offset, limit);
    if (page.books.empty()) {
        std::cout << "No books in this page.\n";
        return;
    }

    std::cout << "Showing " << page.books.size() << " of " << page.total_books << " books.\n";
    for (const auto& book : page.books) {
        print_book(book);
    }

    if (page.has_next) {
        std::cout << "More books are available.\n";
    }
}

void list_available_books(const library::LibraryService& service) {
    const auto books = service.available_books();

    if (books.empty()) {
        std::cout << "No available books.\n";
        return;
    }

    for (const auto& book : books) {
        print_book(book);
    }
}

void search_books_by_author(const library::LibraryService& service) {
    std::string keyword = read_line("Author keyword: ");

    const auto matches = service.search_by_author(keyword);

    std::cout << "Matches: " << matches.size() << "\n";

    for (const auto& book : matches) {
        print_book(book);
    }
}

void search_books_by_year(const library::LibraryService& service) {
    int year{};
    std::cout << "Publication year: ";

    if (!read_int(year)) {
        return;
    }

    const auto matches = service.search_by_year(year);
    std::cout << "Matches: " << matches.size() << "\n";

    for (const auto& book : matches) {
        print_book(book);
    }
}

void search_by_author_and_year(const library::LibraryService& service) {
    std::string keyword = read_line("Author keyword: ");

    int year{};
    std::cout << "Publication year: ";

    if (!read_int(year)) {
        return;
    }

    const auto matches = service.search_by_author_and_year(keyword, year);
    std::cout << "Matches: " << matches.size() << "\n";

    for (const auto& book : matches) {
        print_book(book);
    }
}

void search_by_borrowed(const library::LibraryService& service) {
    int choice{};

    std::cout << "Enter 1 for borrowed, 0 for available: ";
    if (!read_int(choice) || (choice != 0 && choice != 1)) {
        std::cout << "Please enter 1 or 0.\n";
        return;
    }

    const bool borrowed = choice == 1;
    const auto matches = service.search_by_borrowed(borrowed);
    std::cout << "Matches: " << matches.size() << "\n";

    for (const auto& book : matches) {
        print_book(book);
    }
}

void search_by_author_and_borrowed(const library::LibraryService& service) {
    std::string keyword = read_line("Author keyword: ");

    int choice{};
    std::cout << "Enter 1 for borrowed, 0 for available: ";

    if (!read_int(choice) || (choice != 0 && choice != 1)) {
        std::cout << "Please enter 1 or 0.\n";
        return;
    }

    const bool borrowed = choice == 1;
    const auto matches = service.search_by_author_and_borrowed(keyword, borrowed);
    std::cout << "Matches: " << matches.size() << "\n";

    for (const auto& book : matches) {
        print_book(book);
    }
}

void filter_books_in_library(const library::LibraryService& service) {
    library::BookFilter filter;

    const std::string title = read_line("Title keyword (empty for any): ");
    if (!title.empty()) {
        filter.title_keyword = title;
    }

    const std::string author = read_line("Author keyword (empty for any): ");
    if (!author.empty()) {
        filter.author_keyword = author;
    }

    int year{};
    std::cout << "Publication year (0 for any): ";
    if (!read_int(year)) {
        return;
    }

    if (year < 0) {
        std::cout << "Please enter a non-negative year.\n";
        return;
    }

    if (year != 0) {
        filter.publication_year = year;
    }

    int borrowed_choice{};
    std::cout << "Borrowed status (-1 for any, 0 for available, 1 for borrowed): ";
    if (!read_int(borrowed_choice)) {
        return;
    }

    if (borrowed_choice == 0 || borrowed_choice == 1) {
        filter.borrowed = borrowed_choice == 1;
    } else if (borrowed_choice != -1) {
        std::cout << "Please enter -1, 0, or 1.\n";
        return;
    }

    const auto matches = service.filter_books(filter);
    std::cout << "Matches: " << matches.size() << "\n";

    for (const auto& book : matches) {
        print_book(book);
    }
}

bool save_library(const library::LibraryService& service, const std::string& file_path) {
    const auto status = library::save_books_to_file(service.all_books(), file_path);

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

bool load_library(library::LibraryService& service, const std::string& file_path) {
    const auto result = library::load_books_from_file(file_path);

    switch (result.status) {
    case library::LoadStatus::success:
        for (const auto& book : result.books) {
            service.add_book(book);
        }
        std::cout << "Loaded " << result.books.size() << " book(s).\n";
        return true;
    case library::LoadStatus::file_not_found:
        std::cout << "Books file not found. Starting with an empty library.\n";
        return true;
    case library::LoadStatus::open_error:
        std::cerr << "Failed to open books file.\n";
        return false;
    case library::LoadStatus::invalid_format:
        std::cerr << "Books file has invalid format.\n";
        return false;
    default:
        std::cerr << "Unknown books loading error.\n";
        return false;
    }
}

void show_statistics(const library::LibraryService& service) {
    library::BookFilter filter;

    const std::string title = read_line("Title keyword (empty for any): ");
    if (!title.empty()) {
        filter.title_keyword = title;
    }

    const std::string author = read_line("Author keyword (empty for any): ");
    if (!author.empty()) {
        filter.author_keyword = author;
    }

    int year{};
    std::cout << "Publication year (0 for any): ";
    if (!read_int(year)) {
        return;
    }

    if (year < 0) {
        std::cout << "Please enter a non-negative year.\n";
        return;
    }

    if (year != 0) {
        filter.publication_year = year;
    }

    int choice{};
    std::cout << "Statistics mode (0 all, 1 available, 2 borrowed): ";
    if (!read_int(choice)) {
        return;
    }

    if (choice < 0 || choice > 2) {
        std::cout << "Please enter 0, 1, or 2.\n";
        return;
    }

    if (choice != 0) {
        filter.borrowed = choice == 2;
    }

    const bool has_filter = !title.empty() || !author.empty() || year != 0 || choice != 0;
    const auto stats = has_filter ? service.statistics(filter) : service.statistics();

    std::cout << "total_count: " << stats.total_count << '\n';
    std::cout << "available_count: " << stats.available_count << '\n';
    std::cout << "borrowed_count: " << stats.borrowed_count << '\n';

    const double borrowed_rate =
        stats.total_count == 0
        ? 0.0
        : 100.0 * static_cast<double>(stats.borrowed_count) / static_cast<double>(stats.total_count);
    std::cout << std::fixed << std::setprecision(1) << "borrowed_rate: " << borrowed_rate << "%\n";
}

MenuAction handle_menu_choice(int choice, library::LibraryService& service,
                              const std::string& file_path) {
    switch (static_cast<MenuChoice>(choice)) {
    case MenuChoice::exit:
        return save_library(service, file_path) ? MenuAction::exit_success
                                                : MenuAction::exit_failure;
    case MenuChoice::list_books:
        list_books(service);
        break;
    case MenuChoice::add_book:
        add_book(service);
        break;
    case MenuChoice::borrow_book:
        update_borrow_status(service, true);
        break;
    case MenuChoice::return_book:
        update_borrow_status(service, false);
        break;
    case MenuChoice::remove_book:
        remove_book_from_library(service);
        break;
    case MenuChoice::search_title:
        search_books_by_title(service);
        break;
    case MenuChoice::find_by_id:
        find_book_by_id(service);
        break;
    case MenuChoice::list_by_year:
        list_books_by_year(service);
        break;
    case MenuChoice::list_available:
        list_available_books(service);
        break;
    case MenuChoice::search_author:
        search_books_by_author(service);
        break;
    case MenuChoice::search_year:
        search_books_by_year(service);
        break;
    case MenuChoice::search_author_and_year:
        search_by_author_and_year(service);
        break;
    case MenuChoice::search_borrowed:
        search_by_borrowed(service);
        break;
    case MenuChoice::search_author_and_borrowed:
        search_by_author_and_borrowed(service);
        break;
    case MenuChoice::update_book:
        update_book_in_library(service);
        break;
    case MenuChoice::list_by_title:
        list_books_by_title(service);
        break;
    case MenuChoice::list_page:
        list_books_page(service);
        break;
    case MenuChoice::filter_books:
        filter_books_in_library(service);
        break;
    case MenuChoice::show_statistics:
        show_statistics(service);
        break;
    case MenuChoice::save_now:
        if (save_library(service, file_path)) {
            std::cout << "Books saved.\n";
        }
        break;
    default:
        std::cout << "Unknown choice.\n";
        break;
    }

    return MenuAction::continue_running;
}

} // namespace

int main(int argc, char* argv[]) { // NOLINT(bugprone-exception-escape): iostream owns this boundary.
    if (argc > 2) {
        std::cerr << "Usage: library_cli [books_file]\n";
        return 1;
    }
    const std::string file_path = argc == 2 ? argv[1] : "books.txt";
    library::LibraryService service;

    if (!load_library(service, file_path)) {
        return 1;
    }

    while (true) {
        print_menu();
        int choice{};
        if (!read_int(choice)) {
            if (std::cin.eof()) {
                return save_library(service, file_path) ? 0 : 1;
            }

            continue;
        }

        const MenuAction action = handle_menu_choice(choice, service, file_path);

        if (action == MenuAction::exit_success) {
            return 0;
        }

        if (action == MenuAction::exit_failure) {
            return 1;
        }
    }
}
