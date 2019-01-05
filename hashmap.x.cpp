// stl
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <random>

// google/benchmark
#include <benchmark/benchmark.h>

//
#include <absl/container/flat_hash_map.h>

static const std::int64_t SEED = 0;
static std::mt19937_64 generator(SEED);


template <typename KeyType, typename ValueType, template<typename ...> typename HashMap>
struct Adapter
{
    using Key = KeyType;
    using Value = ValueType;
    using C = HashMap<Key, Value>;
    template <typename ...Args>
    static auto insert(C& c, KeyType k, ValueType v) { return c.insert({k, v});}
};

// template<typename KeyType, typename ValueType>
// struct Adapter<KeyType, ValueType, std::unordered_map>
// {
//     using C = std::unordered_map<KeyType, ValueType>;
//     template <typename ...Args>
//     static auto insert(C& c, KeyType k, ValueType v) { return c.insert({k, v});}
// };

template <typename K, typename V, template<typename ...> typename H>
void BM_Insert_Sequential(benchmark::State& state)
{
    using AdapterT = Adapter<K, V, H>;
    using Type = typename AdapterT::C;
    Type c;
    int64_t i = 0;
    const int64_t value = 42;
    for(auto _ : state)
    {
        state.PauseTiming();
        c.clear();
        c.reserve(state.range(0));
        state.ResumeTiming();
        for (K i= 0; i < state.range(0); ++i)
        {
            AdapterT::insert(c, i++, value);
        }
    }
}

template <typename K, typename V, template<typename ...> typename H>
static void BM_Insert_Random(benchmark::State& state) {
    using AdapterT = Adapter<K, V, H>;
    using Type = typename AdapterT::C;
    Type c;
    int i = 0;
    const int64_t value = 42;
    std::vector<int> keys;
    keys.reserve(state.range(0));
    for(auto _ : state)
    {
        state.PauseTiming();
        keys.clear();
        for(int i = 0; i < state.range(0); ++i)
        {
            keys.push_back(i);
        }
        std::shuffle(keys.begin(), keys.end(), generator);

        c.clear();
        c.reserve(state.range(0));
        state.ResumeTiming();
        for (K i = 0; i < state.range(0); ++i)
        {
            auto key = keys[i];
            AdapterT::insert(c, key, value);
        }
    }
}


BENCHMARK_TEMPLATE(BM_Insert_Sequential, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Sequential, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Sequential, int32_t, int32_t, std::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Sequential, int32_t, int32_t, absl::flat_hash_map)->Arg(1000)->Arg(100000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(100000)->Arg(1000000);;
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(100000)->Arg(1000000);;

BENCHMARK_MAIN();
