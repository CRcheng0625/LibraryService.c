cmake_minimum_required(VERSION 3.20)

project(first_cpp_project VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(library_core
    src/library_service.cpp
    src/book_storage.cpp
)

target_include_directories(library_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(library_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(library_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(library_cli app/main.cpp)
target_link_libraries(library_cli PRIVATE library_core)

add_executable(lookup_benchmark tools/lookup_benchmark.cpp)
target_link_libraries(lookup_benchmark PRIVATE library_core)

include(CTest)

if(BUILD_TESTING)
    add_executable(library_tests tests/library_service_test.cpp)
    target_link_libraries(library_tests PRIVATE library_core)
    add_test(NAME library_service_tests COMMAND library_tests)
endif()

