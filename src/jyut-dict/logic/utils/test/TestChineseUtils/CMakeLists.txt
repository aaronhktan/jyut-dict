cmake_minimum_required(VERSION 3.5)

project(TestChineseUtils LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(TestChineseUtils tst_chineseutils.cpp)
add_test(NAME TestChineseUtils COMMAND TestChineseUtils)

target_link_libraries(TestChineseUtils PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_include_directories(TestChineseUtils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

target_sources(TestChineseUtils
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../chineseutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../settings/settings.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../utils.cpp)

