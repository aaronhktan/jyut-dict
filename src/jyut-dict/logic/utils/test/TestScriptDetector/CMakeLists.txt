cmake_minimum_required(VERSION 3.20)

project(TestScriptDetector LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ScriptDetector tst_scriptdetector.cpp)
add_test(NAME ScriptDetector COMMAND ScriptDetector)

target_link_libraries(ScriptDetector PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_include_directories(ScriptDetector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)

target_sources(ScriptDetector
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../scriptdetector.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../chineseutils.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../utils.cpp
)

