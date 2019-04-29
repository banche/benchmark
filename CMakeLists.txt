cmake_minimum_required (VERSION 3.10)
project(yoshi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.61.0 COMPONENTS program_options REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)

add_subdirectory(thirdparties)
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(yoshi)
