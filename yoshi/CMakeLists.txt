# common library
add_library(yoshi yoshi.cpp)
target_link_libraries(yoshi PUBLIC benchmark)

add_library(yoshi_main yoshi.x.cpp)
target_link_libraries(yoshi_main
    PUBLIC yoshi
    PRIVATE Boost::program_options)

function(yoshi_add_benchmark name)
    # simple function to create a benchmark
    set(options )
    set(oneValueArgs )
    set(multiValueArgs SRC DEPENDS)
    cmake_parse_arguments(_add
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN})

    add_executable(${name} ${_add_SRC})
    target_link_libraries(${name}
        ${_add_DEPENDS}
        yoshi_main)
endfunction()

# benchmarks folder
add_subdirectory(hashmap)
