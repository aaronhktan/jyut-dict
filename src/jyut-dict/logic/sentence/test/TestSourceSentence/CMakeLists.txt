cmake_minimum_required(VERSION 3.20)

project(SourceSentence LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(SourceSentence tst_sourcesentence.cpp)
add_test(NAME SourceSentence COMMAND SourceSentence)

target_link_libraries(SourceSentence PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_include_directories(SourceSentence PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

target_sources(SourceSentence
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../sentenceset.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../sourcesentence.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../dictionary/dictionarysource.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/chineseutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/utils.cpp
)
