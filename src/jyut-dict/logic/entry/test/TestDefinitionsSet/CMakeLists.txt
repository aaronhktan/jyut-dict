cmake_minimum_required(VERSION 3.5)

project(TestDefinitionsSet LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TestDefinitionsSet tst_definitionsset.cpp)
add_test(NAME TestDefinitionsSet COMMAND TestDefinitionsSet)

target_link_libraries(TestDefinitionsSet PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_include_directories(TestDefinitionsSet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

target_sources(TestDefinitionsSet
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../definitionsset.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../dictionary/dictionarysource.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../sentence/sentenceset.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../sentence/sourcesentence.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/chineseutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/utils.cpp
)

