cmake_minimum_required(VERSION 3.5)

project(TestEntry LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TestEntry tst_entry.cpp)
add_test(NAME TestEntry COMMAND TestEntry)

target_link_libraries(TestEntry PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_include_directories(TestEntry PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

target_sources(TestEntry
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../dictionary/dictionarysource.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../entry/definitionsset.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../entry/entry.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/chineseutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/utils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../sentence/sentenceset.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../sentence/sourcesentence.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../settings/settings.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../settings/settingsutils.cpp
)

