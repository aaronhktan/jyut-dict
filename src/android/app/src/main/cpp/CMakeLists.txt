
cmake_minimum_required(VERSION 3.22)

project(sqlite3pcre2 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# PCRE2 configuration
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)

set(PCRE2_BUILD_PCRE2_8 ON CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2_16 OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2_32 OFF CACHE BOOL "" FORCE)

set(PCRE2_BUILD_PCRE2GREP OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2TEST OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_TESTS OFF CACHE BOOL "" FORCE)

set(PCRE2_SUPPORT_JIT ON CACHE BOOL "" FORCE)

# Build PCRE2
add_subdirectory(
        vendor/pcre2
        ${CMAKE_CURRENT_BINARY_DIR}/pcre2
)

# Build SQLite PCRE2 extension
add_library(sqlite3pcre2 SHARED
        vendor/sqlite3-pcre2/pcre2.c
)

# SQLite headers
target_include_directories(sqlite3pcre2 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/sqlite
)

# Link PCRE2
target_link_libraries(sqlite3pcre2 PRIVATE
        pcre2-8-static
)

set_target_properties(sqlite3pcre2 PROPERTIES
        OUTPUT_NAME "sqlite3pcre2"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)
