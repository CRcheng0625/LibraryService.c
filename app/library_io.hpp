#pragma once

#include "library/library_service.hpp"

#include <string>

namespace library_io {

bool save_library(const library::LibraryService& service, const std::string& file_path);

bool load_library(library::LibraryService& service,
                  const std::string& file_path,
                  bool show_status = true);

bool check_library_file(const std::string& file_path);

} // namespace library_io
