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
- [folly::F14FastMap](https://github.com/facebook/folly/blob/master/folly/container/F14.md)

Here is an example taken from the generated output for the last benchmarks. It measure
the time it takes to randomly insert and after few other operations (insert and erase)
erase the element.

![example](https://github.com/banche/benchmark/blob/master/result_insert_erase_random.png "Random Insert/Erase performances")

## Requirements

The repository is using submodules for thirparties libraries so make sure
you also clone the submodules before running `cmake`

## Run the benchmarks

The project is using CMake build-system, here is an example to configure, build and run the benchmarks
```bash
$ mkdir -p build
$ cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_TESTING=OFF . build
$ cd build && make -j12
$ cd -
```

Once everything is build you can just run the benchmark and wait for the results to come! You will need `python3` and `jinja2` package installed to generate the plots

```bash
$ build/yoshi/hashmap/hashmap --benchmark_min_time=2 --benchmark_format=json | python3 benchmark.py
```
Once this is done this will open a web browser with all the plots in one page.

## TODO

- complete the benchmark list : do more relevant tests cases (find miss/hit for example)
- try to make benchmark.py more generic
    - it only works for cases called with `BENCHMARK_TEMPLATE()->Arg`
    - it should support several input files
- add benchmarks for other containers
- add short and long mode when running the benchmark and also a size to use
