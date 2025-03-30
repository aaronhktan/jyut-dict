cmake_minimum_required(VERSION 3.5)

project(TestSentenceSet LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SentenceSet tst_sentenceset.cpp)
add_test(NAME TestSentenceSet COMMAND TestSentenceSet)

target_link_libraries(SentenceSet PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_include_directories(SentenceSet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

target_sources(SentenceSet
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../sentenceset.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../dictionary/dictionarysource.cpp
)
