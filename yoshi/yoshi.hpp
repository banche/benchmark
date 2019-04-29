#pragma once

#include <benchmark/benchmark.h>

#include <functional>
#include <memory>
#include <vector>

namespace yoshi {
namespace internal {
/// Stores a benchmark to add it later on the Google's benchmarks
struct Benchmark
{
    using Function = std::function<void(benchmark::State&)>;
    std::string name;
    Function benchmark;
    Benchmark(const char* n, Function f)
        : name(n)
        , benchmark(f)
    {
    }
};

/// Benchmark pointer type
using BenchmarkPtr = std::unique_ptr<Benchmark>;

/// Holder class
class Benchmarks
{
public:
    Benchmarks() = delete;
    /// Adds the benchmark in the list
    static Benchmark* add(Benchmark* b);
    /// Returns all the benchmarks
    static const std::vector<BenchmarkPtr>& get();
private:
    static std::vector<BenchmarkPtr> s_benchmarks;
};

}
}
// Macro to register benchmarks

// Check that __COUNTER__ is defined and that __COUNTER__ increases by 1
// every time it is expanded. X + 1 == X + 0 is used in case X is defined to be
// empty. If X is empty the expression becomes (+1 == +0).
#if defined(__COUNTER__) && (__COUNTER__ + 1 == __COUNTER__ + 0)
#define YOSHI_PRIVATE_UNIQUE_ID __COUNTER__
#else
#define YOSHI_PRIVATE_UNIQUE_ID __LINE__
#endif

// Helpers for generating unique variable names
#define YOSHI_PRIVATE_NAME(n) \
  YOSHI_PRIVATE_CONCAT(_yoshi_benchmark_, YOSHI_PRIVATE_UNIQUE_ID, n)
#define YOSHI_PRIVATE_CONCAT(a, b, c) YOSHI_PRIVATE_CONCAT2(a, b, c)
#define YOSHI_PRIVATE_CONCAT2(a, b, c) a##b##c

// Helper to add a benchmark
#define YOSHI_ADD_BENCHMARK(f, ...)                                 \
    static yoshi::internal::Benchmark*                              \
        YOSHI_PRIVATE_NAME(f) = yoshi::internal::Benchmarks::add(   \
        new yoshi::internal::Benchmark(                             \
            #f "<" #__VA_ARGS__ ">",                                \
            [=](auto& st) { f<__VA_ARGS__>(st);}));
