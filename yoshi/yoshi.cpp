#include "yoshi.hpp"

#include <iostream>

namespace yoshi {
namespace internal {
std::vector<BenchmarkPtr> Benchmarks::s_benchmarks;

Benchmark* Benchmarks::add(Benchmark* b)
{
    std::cout << "Creating! " << b->name << "\n";
    s_benchmarks.emplace_back(b);
    std::cout << s_benchmarks.size() << "\n";
    return s_benchmarks.back().get();
}

const std::vector<BenchmarkPtr>& Benchmarks::get()
{
    return s_benchmarks;
}
}
}
