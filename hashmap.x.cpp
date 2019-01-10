// stl
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <random>

// google/benchmark
#include <benchmark/benchmark.h>

// absl
#include <absl/container/flat_hash_map.h>

// Boost
#include <boost/unordered_map.hpp>

static const std::int64_t SEED = 0;
static std::mt19937_64 generator(SEED);


template <typename KeyType, typename ValueType, template<typename ...> typename HashMap>
struct Adapter
{
    using Key = KeyType;
    using Value = ValueType;
    using C = HashMap<Key, Value>;

    static auto insert(C& c, KeyType k, ValueType v) { return c.insert({k, v});}
    static auto erase(C& c, KeyType k) { return c.erase(k); }
    static auto find(const C& c, KeyType k) { return c.find(k); }
    static void reserve(C& c, std::size_t size) { c.reserve(size); }
    static void clear(C& c) { c.clear(); }
    static auto begin(C& c) { return c.begin(); }
    static auto end(C& c) { return c.end(); }
};

template <typename V>
struct ValueSelector
{
};

template<>
struct ValueSelector<int64_t>
{
    static int64_t value() { return 42; }
};

template<>
struct ValueSelector<int32_t>
{
    static auto value() { return 42; }
};

template<>
struct ValueSelector<std::string>
{
    static auto value() { return "moizouzoumoithisisalongstringreallylong"; }
};

template <typename K, typename V, template<typename ...> typename H>
void BM_Insert_Sequential(benchmark::State& state)
{
    using AdapterT = Adapter<K, V, H>;
    using Type = typename AdapterT::C;
    Type c;
    const auto value = ValueSelector<V>::value();
    for(auto _ : state)
    {
        state.PauseTiming();
        AdapterT::clear(c);
        AdapterT::reserve(c, state.range(0));
        state.ResumeTiming();
        for (K i= 0; i < state.range(0); ++i)
        {
            AdapterT::insert(c, i, value);
        }
    }
}

template <typename K, typename V, template<typename ...> typename H>
static void BM_Insert_Random(benchmark::State& state)
{
    using AdapterT = Adapter<K, V, H>;
    using Type = typename AdapterT::C;
    Type c;
    auto value = ValueSelector<V>::value();
    std::vector<K> keys;
    keys.reserve(state.range(0));
    for(auto _ : state)
    {
        state.PauseTiming();
        keys.clear();
        for(K i = 0; i < state.range(0); ++i)
        {
            keys.push_back(i);
        }
        std::shuffle(keys.begin(), keys.end(), generator);

        AdapterT::clear(c);
        AdapterT::reserve(c, state.range(0));
        state.ResumeTiming();
        for (K i = 0; i < state.range(0); ++i)
        {
            auto key = keys[i];
            AdapterT::insert(c, key, value);
        }
    }
}

template <typename K, typename V, template<typename ...> typename H>
static void BM_Erase_Sequential(benchmark::State& state)
{
    using AdapterT = Adapter<K, V, H>;
    using Type = typename AdapterT::C;
    Type c;
    auto value = ValueSelector<V>::value();
    for(auto _ : state)
    {
        state.PauseTiming();
        AdapterT::clear(c);
        AdapterT::reserve(c, state.range(0));
        for (K i= 0; i < state.range(0); ++i)
        {
            AdapterT::insert(c, i, value);
        }
        state.ResumeTiming();
        for (K i= 0; i < state.range(0); ++i)
        {
            AdapterT::erase(c, i);
        }
    }
}

template <typename K, typename V, template<typename ...> typename H>
static void BM_Erase_Random(benchmark::State& state)
{
    using AdapterT = Adapter<K, V, H>;
    using Type = typename AdapterT::C;
    Type c;
    auto value = ValueSelector<V>::value();
    std::vector<K> keys;
    keys.reserve(state.range(0));
    for(auto _ : state)
    {
        state.PauseTiming();
        keys.clear();
        for(K i = 0; i < state.range(0); ++i)
        {
            keys.push_back(i);
        }
        std::shuffle(keys.begin(), keys.end(), generator);
        AdapterT::clear(c);
        AdapterT::reserve(c, state.range(0));
        for (K i= 0; i < state.range(0); ++i)
        {
            auto key = keys[i];
            AdapterT::insert(c, key, value);
        }
        // shuffle again
        std::shuffle(keys.begin(), keys.end(), generator);
        state.ResumeTiming();
        for (K i= 0; i < state.range(0); ++i)
        {
            auto key = keys[i];
            AdapterT::erase(c, key);
        }
    }
}


BENCHMARK_TEMPLATE(BM_Insert_Sequential, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Sequential, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Sequential, int64_t, int64_t, boost::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_Insert_Sequential, int32_t, int32_t, std::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Sequential, int32_t, int32_t, absl::flat_hash_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Sequential, int32_t, int32_t, boost::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);;
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(100000)->Arg(1000000);;
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, int64_t, boost::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);;
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, std::string, std::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);;
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, std::string, absl::flat_hash_map)->Arg(1000)->Arg(100000)->Arg(1000000);;
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, std::string, boost::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);;

BENCHMARK_TEMPLATE(BM_Erase_Sequential, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Erase_Sequential, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Erase_Sequential, int64_t, int64_t, boost::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_Erase_Random, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Erase_Random, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Erase_Random, int64_t, int64_t, boost::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);


BENCHMARK_MAIN();
