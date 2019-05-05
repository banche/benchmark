#include "yoshi.hpp"

#include <iostream>

namespace yoshi {
namespace internal {
Benchmark* add(Benchmark* b)
{
    return Benchmarks::instance()->add(b);
}

Benchmark* Benchmarks::add(Benchmark* b)
{
    m_benchmarks.emplace_back(b);
    return m_benchmarks.back().get();
}

const std::vector<BenchmarkPtr>& Benchmarks::get()
{
    return m_benchmarks;
}

Benchmarks* Benchmarks::instance()
{
    static Benchmarks s_instance;
    return &s_instance;
}
}
}
