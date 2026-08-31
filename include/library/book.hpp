#pragma once

#include <string>

namespace library {

struct Book {
    int id{};
    std::string title;
    std::string author;
    int publication_year{};
    bool borrowed{};
};

} // namespace library
