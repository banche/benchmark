cmake_minimum_required (VERSION 3.10)
project(micro-benchmarks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.61.0 REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)

add_subdirectory(thirdparties)
include_directories(${CMAKE_SOURCE_DIR})

set(BENCHMARKS_SRC
    benchmark/absl_flat_map.cpp
    benchmark/boost_unordered_map.cpp
    benchmark/qt5_hash.cpp
    benchmark/std_unordered_map.cpp
    benchmark/ska.cpp
    benchmark/tsl_robin_map.cpp
    benchmark/folly.cpp
)

add_executable(hashmap hashmap.x.cpp ${BENCHMARKS_SRC})
target_link_libraries(hashmap
    benchmark
    absl::flat_hash_map
    Qt5::Core
    tsl::robin_map
    folly
)
