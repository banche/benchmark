# Benchmark

The purpose of this repository is to provide some C++ micro-benchmarks on common
datastructures, to help us decide which one to use in which cases.

## Hashmap

Lot's of hashmap implementations are available and in proprietary code you
can often find custom implementations (for the best or worst) `hashmap.x.cpp`
benchmarks right now
- std::unordered_map
- absl::unordered_map
- boost::unordered_map

## TODO

- complete the benchmark list : find, iterate over the container
- add tools to
    - parse google benchmark output
    - generate graphs
- add benchmarks for other containers
- add more thirdparties libraries
