# Benchmark

The purpose of this repository is to provide some C++ micro-benchmarks on common
datastructures, to help us decide which one to use in which cases.

All the micro-benchmarks are done using [google-benchmark](https://github.com/google/benchmark)

## Hashmap

Lot's of hashmap implementations are available and in proprietary code you
can often find custom implementations (for the best or worst) `hashmap.x.cpp`
benchmarks right now
- std::unordered_map
- [absl::unordered_map](https://abseil.io/)
- boost::unordered_map
- QHash
- [ska::flat_hash_map](https://github.com/skarupke/flat_hash_map)
- [tsl::robin_map](https://github.com/Tessil/robin-map)

Here is an example taken from the generated output for the last benchmarks. It measure
the time it takes to randomly insert and after few other operations (insert and erase)
erase the element.

![example](https://github.com/banche/benchmark/blob/master/result_insert_erase_random.png "Random Insert/Erase performances")


## TODO

- complete the benchmark list : do more relevant tests cases (find miss/hit for example)
- try to make benchmark.py more generic
    - it only works for cases called with `BENCHMARK_TEMPLATE()->Arg`
    - it should support several input files
    - it should be able to read from stdin
- add benchmarks for other containers
