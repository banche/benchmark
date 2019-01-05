cmake_minimum_required (VERSION 3.10)
project(micro-benchmarks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.61.0 REQUIRED)

add_subdirectory(thirdparties)


add_executable(hashmap hashmap.x.cpp)
target_link_libraries(hashmap benchmark absl::flat_hash_map)
