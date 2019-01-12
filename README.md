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

Here is an example taken from the jupyter notebook for the last benchmarks. It measure
the time it takes to randomly insert and after few other operations (insert and erase)
erase the element.

![alt text](https://github.com/banche/benchmark/insert_erase.png "Random Insert/Erase performances")

## TODO

- complete the benchmark list : find, iterate over the container
- add tools to
    - parse google benchmark output
    - generate graphs
- add benchmarks for other containers
- add more thirdparties libraries
