cmake_minimum_required(VERSION 3.20)

project(TestMandarinUtils LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(TestMandarinUtils tst_mandarinutils.cpp)
add_test(NAME TestMandarinUtils COMMAND TestMandarinUtils)

target_link_libraries(TestMandarinUtils PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_include_directories(TestMandarinUtils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

target_sources(TestMandarinUtils
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../mandarinutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../settings/settings.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../utils.cpp)

