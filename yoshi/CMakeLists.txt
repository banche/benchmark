# common library
add_library(yoshi yoshi.cpp)
target_link_libraries(yoshi PUBLIC benchmark)

add_library(yoshi_main yoshi.x.cpp)
target_link_libraries(yoshi_main
    PUBLIC yoshi
    PRIVATE Boost::program_options)

# benchmarks folder
add_subdirectory(hashmap)
