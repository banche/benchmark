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
