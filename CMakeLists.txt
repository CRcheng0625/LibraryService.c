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
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    include(GoogleTest)

    add_executable(library_tests tests/library_service_test.cpp)
    target_link_libraries(library_tests PRIVATE library_core)
    add_test(NAME library_service_tests COMMAND library_tests)

    add_executable(library_gtest tests/library_service_gtest.cpp)
    target_link_libraries(library_gtest PRIVATE library_core GTest::gtest_main)
    gtest_discover_tests(library_gtest)

    add_executable(book_storage_tests tests/book_storage_test.cpp)
    target_link_libraries(book_storage_tests PRIVATE library_core)
    add_test(NAME book_storage_tests COMMAND book_storage_tests)
endif()

