set(BENCHMARKS_SRC
    absl_flat_map.cpp
    boost_unordered_map.cpp
    qt5_hash.cpp
    std_unordered_map.cpp
    ska.cpp
    tsl_robin_map.cpp
    folly.cpp
)

add_executable(hashmap ${BENCHMARKS_SRC})
target_link_libraries(hashmap
    absl::flat_hash_map
    Qt5::Core
    tsl::robin_map
    folly
    yoshi_main
)

yoshi_add_benchmark(hashmap_absl
    SRC absl_flat_map.cpp
    DEPENDS absl::flat_hash_map)

yoshi_add_benchmark(hashmap_std
    SRC std_unordered_map.cpp)

yoshi_add_benchmark(hashmap_boost
    SRC boost_unordered_map.cpp)

yoshi_add_benchmark(hashmap_ska
    SRC ska.cpp)

yoshi_add_benchmark(hashmap_folly
    SRC folly.cpp
    DEPENDS folly)

yoshi_add_benchmark(hashmap_qt5
    SRC qt5_hash.cpp
    DEPENDS Qt5::Core)
