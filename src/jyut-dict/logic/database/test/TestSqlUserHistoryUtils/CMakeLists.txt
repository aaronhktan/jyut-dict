cmake_minimum_required(VERSION 3.5)

project(TestSqlUserHistoryUtils LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Sql Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(TestSqlUserHistoryUtils tst_sqluserhistoryutils.cpp)
add_test(NAME TestSqlUserHistoryUtils COMMAND TestSqlUserHistoryUtils)

target_link_libraries(TestSqlUserHistoryUtils
    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
    PRIVATE Qt${QT_VERSION_MAJOR}::Test
)
target_include_directories(TestSqlUserHistoryUtils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)


set_target_properties(TestSqlUserHistoryUtils PROPERTIES
    MACOSX_BUNDLE TRUE
)

target_sources(TestSqlUserHistoryUtils
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../sqluserhistoryutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../queryparseutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../sqldatabasemanager.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../sqldatabaseutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../dictionary/dictionarymetadata.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../dictionary/dictionarysource.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../entry/definitionsset.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../entry/entry.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../sentence/sentenceset.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../sentence/sourcesentence.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../settings/settings.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/chineseutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/scriptdetector.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/utils.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -DPORTABLE")
