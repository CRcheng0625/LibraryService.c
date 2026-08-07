#include "library/library_service.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <utility>

namespace {

    bool read_int(int& value) {
        if (std::cin >> value) {
            return true;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a number.\n";
        return false;
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
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Title: ";
        std::getline(std::cin, book.title);
        std::cout << "Author: ";
        std::getline(std::cin, book.author);
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
        std::string keyword;
        std::cout << "Title keyword: ";

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(std::cin, keyword);

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
        if (!book.has_value()) {
            std::cout << "Book not found.\n";
            return;
        }

        print_book(book.value());
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
}  // namespace

int main() {
    library::LibraryService service;
    service.add_book({1, "The C++ Programming Language", "Bjarne Stroustrup", 2013, false});

    while (true) {
        print_menu();
        int choice{};
        if (!read_int(choice)) {
            continue;
        }

        switch (choice) {
        case 0:
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
        default:
            std::cout << "Unknown choice.\n";
        }
    }
}
