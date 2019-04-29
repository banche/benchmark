# common library
add_library(yoshi yoshi.cpp)
target_link_libraries(yoshi PUBLIC benchmark)

# benchmarks folder
add_subdirectory(hashmap)
