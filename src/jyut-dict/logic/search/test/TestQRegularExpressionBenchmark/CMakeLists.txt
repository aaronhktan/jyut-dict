cmake_minimum_required(VERSION 3.20)

project(TestQRegularExpressionBenchmark LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Sql Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(TestQRegularExpressionBenchmark tst_qregularexpressionbenchmark.cpp)
add_test(NAME TestQRegularExpressionBenchmark COMMAND TestQRegularExpressionBenchmark)

target_link_libraries(TestQRegularExpressionBenchmark
    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
    PRIVATE Qt${QT_VERSION_MAJOR}::Test
)
target_include_directories(TestQRegularExpressionBenchmark PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

set_target_properties(TestQRegularExpressionBenchmark PROPERTIES
    MACOSX_BUNDLE TRUE
)

target_sources(TestQRegularExpressionBenchmark
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/cantoneseutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/chineseutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/mandarinutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/utils.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -DPORTABLE")
