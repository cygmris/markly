cmake_minimum_required(VERSION 3.20)

project(Markly
    VERSION 1.0.0
    DESCRIPTION "A pleasant note-taking platform (Qt 6 rewrite of VNote)"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Generate compile_commands.json for tooling.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets Network Sql Qml Quick QuickWidgets)

qt_standard_project_setup()

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
