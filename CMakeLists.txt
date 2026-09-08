cmake_minimum_required(VERSION 3.20)

project(first_cpp_project VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(library_core
    src/library_service.cpp
    src/book_storage.cpp
    src/book_validation.cpp
)

target_include_directories(library_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(library_core PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(library_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(
    library_cli
    app/main.cpp
    app/cli_option.cpp
    app/library_io.cpp
)

target_compile_definitions(library_cli PRIVATE
    LIBRARY_CLI_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(library_cli PRIVATE library_core)

add_executable(lookup_benchmark tools/lookup_benchmark.cpp)
target_link_libraries(lookup_benchmark PRIVATE library_core)

option(FIRST_CPP_ENABLE_MYSQL "Build MySQL repository support and connection check" OFF)

if(FIRST_CPP_ENABLE_MYSQL)
    set(FIRST_CPP_MYSQL_CONNECTOR_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mysql-connector-cpp"
        CACHE PATH "MySQL Connector/C++ installation directory"
    )

    find_package(mysql-concpp CONFIG REQUIRED
        PATHS "${FIRST_CPP_MYSQL_CONNECTOR_DIR}"
        NO_DEFAULT_PATH
    )

    add_executable(mysql_connection_check tools/mysql_connection_check.cpp)
    if(MSVC)
        target_compile_options(mysql_connection_check PRIVATE /EHsc)
    endif()
    target_sources(mysql_connection_check PRIVATE src/mysql_book_repository.cpp)
    target_include_directories(mysql_connection_check PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(mysql_connection_check PRIVATE library_core mysql::concpp)

    target_sources(library_cli PRIVATE src/mysql_book_repository.cpp)
    if(MSVC)
        target_compile_options(library_cli PRIVATE /EHsc)
    endif()
    target_compile_definitions(library_cli PRIVATE FIRST_CPP_ENABLE_MYSQL)
    target_link_libraries(library_cli PRIVATE mysql::concpp)

    add_custom_command(TARGET library_cli POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${MYSQL_CONCPP_RUNTIME_LIBRARY_DIR}"
            "$<TARGET_FILE_DIR:library_cli>"
    )

    add_custom_command(TARGET mysql_connection_check POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${MYSQL_CONCPP_RUNTIME_LIBRARY_DIR}"
            "$<TARGET_FILE_DIR:mysql_connection_check>"
    )
endif()

include(GNUInstallDirs)

install(
    TARGETS library_cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    FILES tests/data/books.txt
    DESTINATION ${CMAKE_INSTALL_DATADIR}/first_cpp_project
)

install(
    FILES database/schema.sql
    DESTINATION ${CMAKE_INSTALL_DATADIR}/first_cpp_project/database
)

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "first_cpp_project")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_FILE_NAME "first_cpp_project-${PROJECT_VERSION}")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY TRUE)
include(CPack)

include(CTest)

if(BUILD_TESTING)
    include(FetchContent)

    # GoogleTest is needed to build tests, but is not part of the application package.
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(INSTALL_GMOCK OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    include(GoogleTest)

    add_executable(library_gtest tests/library_service_gtest.cpp)
    target_link_libraries(library_gtest PRIVATE library_core GTest::gtest_main)
    gtest_discover_tests(library_gtest)

    add_executable(book_storage_gtest tests/book_storage_gtest.cpp)
    target_link_libraries(book_storage_gtest PRIVATE library_core GTest::gtest_main)
    gtest_discover_tests(book_storage_gtest)

    add_executable(book_validation_gtest tests/book_validation_gtest.cpp)
    target_link_libraries(book_validation_gtest PRIVATE library_core GTest::gtest_main)
    gtest_discover_tests(book_validation_gtest)

    add_executable(library_io_gtest tests/library_io_gtest.cpp app/library_io.cpp)
    target_include_directories(library_io_gtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app)
    target_link_libraries(library_io_gtest PRIVATE library_core GTest::gtest_main)
    gtest_discover_tests(library_io_gtest)

    add_test(NAME LibraryCliVersion COMMAND library_cli --version)

    set_tests_properties(LibraryCliVersion PROPERTIES
    PASS_REGULAR_EXPRESSION "version"
    )

    add_test(NAME LibraryCliHelp COMMAND library_cli --help)

    set_tests_properties(LibraryCliHelp PROPERTIES
        PASS_REGULAR_EXPRESSION "Usage:"
    )

    add_test(NAME LibraryCliUnknownOption COMMAND library_cli --unknown)

    set_tests_properties(LibraryCliUnknownOption PROPERTIES
        WILL_FAIL TRUE
    )

    add_test(NAME LibraryCliEmptyPath COMMAND library_cli "")

    set_tests_properties(LibraryCliEmptyPath PROPERTIES
    WILL_FAIL TRUE
    )
    add_test(
    NAME LibraryCliTooManyArguments
    COMMAND library_cli first.txt second.txt
    )

    set_tests_properties(LibraryCliTooManyArguments PROPERTIES
        WILL_FAIL TRUE
    )
    
    add_test(NAME LibraryCliShortHelp COMMAND library_cli -h)

    set_tests_properties(LibraryCliShortHelp PROPERTIES
        PASS_REGULAR_EXPRESSION "Usage:"
    )

    add_test(NAME LibraryCliShortVersion COMMAND library_cli -v)

    set_tests_properties(LibraryCliShortVersion PROPERTIES
        PASS_REGULAR_EXPRESSION "version"
    )

    add_test(
        NAME LibraryCliCheckValidFile
        COMMAND library_cli --check
    )

    set_tests_properties(
        LibraryCliCheckValidFile
        PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "Books file is valid"
     )

add_test(
    NAME LibraryCliCheckInvalidFile
    COMMAND library_cli --check
)

set_tests_properties(
    LibraryCliCheckInvalidFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/invalid"
        WILL_FAIL TRUE
)

add_test(
    NAME LibraryCliCheckCustomFile
    COMMAND library_cli --check custom_books.txt
)

set_tests_properties(
    LibraryCliCheckCustomFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "Loaded 1 book"
)

add_test(
    NAME LibraryCliCheckTooManyArguments
    COMMAND library_cli --check first.txt second.txt
)

set_tests_properties(
    LibraryCliCheckTooManyArguments
    PROPERTIES
        WILL_FAIL TRUE
)

add_test(
    NAME LibraryCliListCustomFile
    COMMAND library_cli --list custom_books.txt
)

set_tests_properties(
    LibraryCliListCustomFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "Custom File Book"
        FAIL_REGULAR_EXPRESSION "Loaded [0-9]+ book"
)

add_test(
    NAME LibraryCliListTooManyArguments
    COMMAND library_cli --list first.txt second.txt
)

set_tests_properties(
    LibraryCliListTooManyArguments
    PROPERTIES
        WILL_FAIL TRUE
)

add_test(
    NAME LibraryCliListDefaultFile
    COMMAND library_cli --list
)

set_tests_properties(
    LibraryCliListDefaultFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "Test Book"
)

add_test(
    NAME LibraryCliListMissingFile
    COMMAND library_cli --list missing_books.txt
)

set_tests_properties(
    LibraryCliListMissingFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "No books yet"
)

add_test(
    NAME LibraryCliListInvalidFile
    COMMAND library_cli --list
)

set_tests_properties(
    LibraryCliListInvalidFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/invalid"
        WILL_FAIL TRUE
)

add_test(
    NAME LibraryCliListInvalidFileOutput
    COMMAND
        ${CMAKE_COMMAND}
        -DCLI_EXECUTABLE=$<TARGET_FILE:library_cli>
        "-DWORKING_DIRECTORY=${CMAKE_CURRENT_SOURCE_DIR}/tests/data/invalid"
        "-DEXPECTED_OUTPUT=Books file has invalid format"
        -DEXPECTED_EXIT_CODE=1
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/check_cli_output.cmake"
)

add_test(
    NAME LibraryCliCountCustomFile
    COMMAND library_cli --count custom_books.txt
)

set_tests_properties(
    LibraryCliCountCustomFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "Book count: 1"
)

add_test(
    NAME LibraryCliCountDefaultFile
    COMMAND library_cli --count
)

set_tests_properties(
    LibraryCliCountDefaultFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "Book count: 2"
)

add_test(
    NAME LibraryCliStatsCustomFile
    COMMAND library_cli --stats custom_books.txt
)

set_tests_properties(
    LibraryCliStatsCustomFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "total_count: 1"
)

add_test(
    NAME LibraryCliAvailableDefaultFile
    COMMAND library_cli --available
)

set_tests_properties(
    LibraryCliAvailableDefaultFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "Test Book"
        FAIL_REGULAR_EXPRESSION "Borrowed Book"
)

add_test(
    NAME LibraryCliAvailableMissingFile
    COMMAND library_cli --available missing_available_books.txt
)

set_tests_properties(
    LibraryCliAvailableMissingFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "No available books"
)

add_test(
    NAME LibraryCliAvailableEmptyFile
    COMMAND library_cli --available empty_books.txt
)

set_tests_properties(
    LibraryCliAvailableEmptyFile
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "No available books"
)

add_test(
    NAME LibraryCliSearchTitle
    COMMAND library_cli --search-title Test books.txt
)

set_tests_properties(
    LibraryCliSearchTitle
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "Matches: 1"
)

add_test(
    NAME LibraryCliSearchAuthor
    COMMAND library_cli --search-author Author books.txt
)

set_tests_properties(
    LibraryCliSearchAuthor
    PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
        PASS_REGULAR_EXPRESSION "Matches: 2"
)

add_test(
    NAME LibraryCliSearchMissingKeyword
    COMMAND library_cli --search-title
)

set_tests_properties(
    LibraryCliSearchMissingKeyword
    PROPERTIES
        WILL_FAIL TRUE
)

add_test(
    NAME LibraryCliSearchTooManyArguments
    COMMAND library_cli --search-author Author books.txt extra.txt
)

set_tests_properties(
    LibraryCliSearchTooManyArguments
    PROPERTIES
        WILL_FAIL TRUE
)

set_property(
    TEST
        LibraryCliVersion
        LibraryCliHelp
        LibraryCliUnknownOption
        LibraryCliEmptyPath
        LibraryCliTooManyArguments
        LibraryCliShortHelp
        LibraryCliShortVersion
        LibraryCliCheckValidFile
        LibraryCliCheckInvalidFile
        LibraryCliCheckCustomFile
        LibraryCliCheckTooManyArguments
        LibraryCliListCustomFile
        LibraryCliListTooManyArguments
        LibraryCliListDefaultFile
        LibraryCliListMissingFile
        LibraryCliListInvalidFile
        LibraryCliListInvalidFileOutput
        LibraryCliCountCustomFile
        LibraryCliCountDefaultFile
        LibraryCliStatsCustomFile
        LibraryCliAvailableDefaultFile
        LibraryCliAvailableMissingFile
        LibraryCliAvailableEmptyFile
        LibraryCliSearchTitle
        LibraryCliSearchAuthor
        LibraryCliSearchMissingKeyword
        LibraryCliSearchTooManyArguments
    PROPERTY LABELS cli
)

endif()

