cmake_minimum_required(VERSION 3.5)

project(TestSqlDatabaseManager LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Sql Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TestSqlDatabaseManager tst_sqldatabasemanager.cpp)
add_test(NAME TestSqlDatabaseManager COMMAND TestSqlDatabaseManager)

target_link_libraries(TestSqlDatabaseManager
    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
    PRIVATE Qt${QT_VERSION_MAJOR}::Test
)
target_include_directories(TestSqlDatabaseManager PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

set_target_properties(TestSqlDatabaseManager PROPERTIES
    MACOSX_BUNDLE TRUE
)

target_sources(TestSqlDatabaseManager
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../sqldatabasemanager.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -DPORTABLE")

