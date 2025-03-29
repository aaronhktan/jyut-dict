cmake_minimum_required(VERSION 3.5)

project(TestSettingsUtils LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TestSettingsUtils tst_settingsutils.cpp)
add_test(NAME TestSettingsUtils COMMAND TestSettingsUtils)

target_link_libraries(TestSettingsUtils PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_include_directories(TestSettingsUtils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

target_sources(TestSettingsUtils
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../settings.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../settingsutils.cpp)

