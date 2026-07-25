#include "library/library_service.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <utility>

namespace {

    void print_menu() {
        std::cout << "\nLibrary manager\n"
            << "1. List books\n"
            << "2. Add book\n"
            << "3. Borrow book\n"
            << "4. Return book\n"
            << "0. Exit\n"
            << "Choose: ";
    }

    void list_books(const library::LibraryService& service) {
        if (service.all_books().empty()) {
            std::cout << "No books yet.\n";
            return;
        }

        for (const auto& book : service.all_books()) {
            std::cout << book.id << " | " << book.title << " | " << book.author << " | "
                << book.publication_year << " | "
                << (book.borrowed ? "borrowed" : "available") << '\n';
        }
    }

    void add_book(library::LibraryService& service) {
        library::Book book;
        std::cout << "Id: ";
        std::cin >> book.id;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Title: ";
        std::getline(std::cin, book.title);
        std::cout << "Author: ";
        std::getline(std::cin, book.author);
        std::cout << "Publication year: ";
        std::cin >> book.publication_year;

        std::cout << (service.add_book(std::move(book)) ? "Book added.\n" : "Invalid or duplicate book.\n");
    }

    void update_borrow_status(library::LibraryService& service, bool borrow) {
        int id{};
        std::cout << "Book id: ";
        std::cin >> id;
        bool success{};
        if (borrow) {
            success = service.borrow_book(id);
        }
        else {
			success = service.return_book(id);
        }
        std::cout << (success ? "Done.\n" : "Operation failed.\n");
        }
    } // namespace

int main() {
    library::LibraryService service;
    service.add_book({1, "The C++ Programming Language", "Bjarne Stroustrup", 2013, false});

    while (true) {
        print_menu();
        int choice{};
        if (!(std::cin >> choice)) {
            std::cout << "Please enter a number.\n";
            return 1;
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
        default:
            std::cout << "Unknown choice.\n";
        }
    }
}
