#include "library/library_service.hpp"
#include "library/book_storage.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <utility>

namespace {

    bool read_int(int& value) {
        if (std::cin >> value) {
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n');
            return true;
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
            << "0. Exit\n"
            << "Choose: ";
    }

    void print_book(const library::Book& book) {
        std::cout << book.id << " | "
            << book.title << " | "
            << book.author << " | "
            << book.publication_year << " | "
            << (book.borrowed ? "borrowed" : "available")
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

        std::cout << (service.add_book(std::move(book)) ? "Book added.\n" : "Invalid or duplicate book.\n");
    }

    void update_borrow_status(library::LibraryService& service, bool borrow) {
        int id{};
        std::cout << "Book id: ";
        if (!read_int(id)) {
            return;
        }
        bool success{};
        if (borrow) {
            success = service.borrow_book(id);
        }
        else {
            success = service.return_book(id);
        }
        std::cout << (success ? "Done.\n" : "Operation failed.\n");
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

    void search_books_by_author(
        const library::LibraryService& service) {
        std::string keyword = read_line("Author keyword: ");

        const auto matches = service.search_by_author(keyword);

        std::cout << "Matches: " << matches.size() << "\n";

        for (const auto& book : matches) {
            print_book(book);
        }
    }

    void search_books_by_year(
        const library::LibraryService& service) {
		int year{};
		std::cout << "Publication year: ";

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n');

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
		int  choice{};

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

    void search_by_author_and_borrowed(
        const library::LibraryService& service) {
        std::string keyword = read_line("Author keyword: ");

        int choice{};
        std::cout << "Enter 1 for borrowed, 0 for available: ";

        if (!read_int(choice) || (choice != 0 && choice != 1)) {
            std::cout << "Please enter 1 or 0.\n";
            return;
        }

        const bool borrowed = choice == 1;
        const auto matches =
            service.search_by_author_and_borrowed(keyword, borrowed);
        std::cout << "Matches: " << matches.size() << "\n";

        for (const auto& book : matches) {
            print_book(book);
        }
    }

}  // namespace

int main() {
    const std::string file_path = "books.txt";
    library::LibraryService service;

    const auto loaded_books = library::load_books_from_file(file_path);
    for (const auto& book : loaded_books) {
        service.add_book(book);
    }

    while (true) {
        print_menu();
        int choice{};
        if (!read_int(choice)) {
            continue;
        }

        switch (choice) {
        case 0:
            if (!library::save_books_to_file(service.all_books(), file_path)) {
                std::cerr << "Failed to save books.\n";
                return 1;
            }
            return 0;
        case 1:
            list_books(service);
            break;
        case 2:
            add_book(service);
            break;
        case 3:
            update_borrow_status(service, true);
            break;
        case 4:
            update_borrow_status(service, false);
            break;
        case 5:
            remove_book_from_library(service);
            break;
        case 6:
            search_books_by_title(service);
            break;
        case 7:
            find_book_by_id(service);
            break;
        case 8:
            list_books_by_year(service);
            break;
        case 9:
            list_available_books(service);
            break;
        case 10:
            search_books_by_author(service);
            break;
        case 11:
            search_books_by_year(service);
            break;
        case 12:
            search_by_author_and_year(service);
            break;
        case 13:
            search_by_borrowed(service);
            break;
        case 14:
            search_by_author_and_borrowed(service);
            break;
        default:
            std::cout << "Unknown choice.\n";
        }
    }
}
