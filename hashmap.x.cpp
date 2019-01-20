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

// Qt
#include <QHash>

static const std::int64_t SEED = 0;
static std::mt19937_64 generator(SEED);

/// Adapter in case hashmap implementation have different interface.
///
/// We assume the default implementation follows the interface of std::unordered_map.
/// If this is not the case, you just need to partially specialize the Adapter.
///
/// Usually we are going to create the default implementation of the hashmap without
/// specifying the hash function to use.
template <typename KeyType, typename ValueType, template<typename ...> typename HashMap>
struct Adapter
{
    using Key = KeyType;
    using Value = ValueType;
    using C = HashMap<Key, Value>;

    static auto insert(C& c, KeyType k, ValueType v) { return c.insert({k, v}).second;}
    static auto erase(C& c, KeyType k) { return c.erase(k); }
    static auto find(const C& c, KeyType k) { return c.find(k); }
    static void reserve(C& c, std::size_t size) { c.reserve(size); }
    static void clear(C& c) { c.clear(); }
    static auto begin(C& c) { return c.begin(); }
    static auto end(C& c) { return c.end(); }
};

template<typename KeyType, typename ValueType>
struct Adapter<KeyType, ValueType, QHash>
{
    using Key = KeyType;
    using Value = ValueType;
    using C = QHash<Key, Value>;

    static auto insert(C& c, KeyType k, ValueType v) { return c.insert(k, v) != c.end();}
    static auto erase(C& c, KeyType k) { return c.remove(k); }
    static auto find(const C& c, KeyType k) { return c.find(k); }
    static void reserve(C& c, std::size_t size) { c.reserve(size); }
    static void clear(C& c) { c.clear(); }
    static auto begin(C& c) { return c.begin(); }
    static auto end(C& c) { return c.end(); }
};

/// Value selection depending on the typ/tag
///
/// Note there is no default implementation, so for each new type/tag this needs to be
/// specialized
template <typename V>
struct ValueSelector
{
};

// TODO : use enable_if to specialize this for all integer type at once
template<>
struct ValueSelector<int64_t>
{
    static auto value() { return 42; }
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

/// Inserts [0, state.range(0) -1] in sequential order
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

/// Inserts [0, state.range(0) -1] in random order
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

/// Inserts [0, state.range(0) -1] in sequential order and measure the time
/// to do the erase inside [0, state.range(0) -1] in a sequential order
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

/// Inserts [0, state.range(0) -1] in random order and measure the time
/// to do the erase inside [0, state.range(0) -1] in a random order (different than insertion)
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

/// Inserts [0, state.range(0) -1] in sequential order and measure the time
/// to do the find inside [0, state.range(0) -1] in a sequential order
template <typename K, typename V, template<typename ...> typename H>
static void BM_Find_Sequential(benchmark::State& state)
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
        int64_t found = 0;
        state.ResumeTiming();
        for (K i= 0; i < state.range(0); ++i)
        {
            auto it = AdapterT::find(c, i);
            found += (it != AdapterT::end(c));
        }
        state.PauseTiming();
        if (found != state.range(0))
        {
            std::string error = std::string("excepted ") + std::to_string(state.range(0));
            error += std::string(" got ") + std::to_string(found);
            throw std::runtime_error(error);
        }
        state.ResumeTiming();
    }
}

/// Inserts [0, state.range(0) -1] in random order and measure the time
/// to do the find in [0, state.range(0) -1] in a random order (different from insert)
template <typename K, typename V, template<typename ...> typename H>
static void BM_Find_Random(benchmark::State& state)
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
        int64_t found = 0;
        state.ResumeTiming();
        for (K i= 0; i < state.range(0); ++i)
        {
            auto key = keys[i];
            auto it = AdapterT::find(c, key);
            found += (it != AdapterT::end(c));
        }
        state.PauseTiming();
        if (found != state.range(0))
        {
            std::string error = std::string("excepted ") + std::to_string(state.range(0));
            error += std::string(" got ") + std::to_string(found);
            throw std::runtime_error(error);
        }
        state.ResumeTiming();
    }
}

enum class Type : uint8_t
{
    NEW,
    DELETE
};

template <typename T>
struct Action
{
    T id;
    Type type;

    bool operator==(const Action<T>& other) const
    {
        return other.id == id and other.type == type;
    }
};
template<typename T>
using Actions = std::vector<Action<T>>;

template<typename T>
Actions<T> generateRandomActions(int64_t count)
{
    Actions<T> result;
    result.reserve(2 * count);
    for(int i = 0; i < count; ++i)
    {
        result.push_back(Action<T>{i, Type::NEW});
    }
    std::shuffle(result.begin(), result.end(), generator);
    std::uniform_int_distribution<T> distribution(1, count);
    for(int i = 0; i < count; ++i)
    {
        auto it = std::find(result.begin() + i, result.end(), Action<T>{i, Type::NEW});
        auto maxRandom = std::distance(it, result.end());
        if (maxRandom <= 1)
        {
            result.push_back(Action<T>{i, Type::DELETE});
        }
        else
        {

            auto insertIt = it + 1 + (distribution(generator) % maxRandom);
            result.insert(insertIt, Action<T>{i, Type::DELETE});
        }
    }
    return result;
}

template <typename K, template<typename ...> typename H>
static void BM_Insert_Erase_Random(benchmark::State& state)
{
    using AdapterT = Adapter<K, Action<K>, H>;
    using CType = typename AdapterT::C;
    CType c;
    auto actions = generateRandomActions<K>(state.range(0));
    for(auto _ : state)
    {
        state.PauseTiming();
        AdapterT::clear(c);
        AdapterT::reserve(c, state.range(0));
        int64_t inserted = 0;
        int64_t deleted = 0;
        state.ResumeTiming();
        for (auto& action : actions)
        {
            if (action.type == Type::NEW)
            {
                inserted += AdapterT::insert(c, action.id, action);
            }
            else if (action.type == Type::DELETE)
            {
                deleted += AdapterT::erase(c, action.id);
            }
        }
        state.PauseTiming();
        if ((inserted != state.range(0)) or (deleted != state.range(0)))
        {
            std::string error = std::string("excepted ") + std::to_string(state.range(0));
            error += std::string(" got ") + std::to_string(inserted);
            error += std::string(" , ") + std::to_string(deleted);
            throw std::runtime_error(error);
        }
        state.ResumeTiming();
    }
}


BENCHMARK_TEMPLATE(BM_Insert_Sequential, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Sequential, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Sequential, int64_t, int64_t, boost::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_Insert_Sequential, int32_t, int32_t, std::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Sequential, int32_t, int32_t, absl::flat_hash_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Insert_Sequential, int32_t, int32_t, boost::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);;
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);;
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, int64_t, boost::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);;
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, std::string, std::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);;
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, std::string, absl::flat_hash_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);;
BENCHMARK_TEMPLATE(BM_Insert_Random, int64_t, std::string, boost::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);;

BENCHMARK_TEMPLATE(BM_Erase_Sequential, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Erase_Sequential, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Erase_Sequential, int64_t, int64_t, boost::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_Erase_Random, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Erase_Random, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Erase_Random, int64_t, int64_t, boost::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_Find_Sequential, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Find_Sequential, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Find_Sequential, int64_t, int64_t, boost::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_Find_Random, int64_t, int64_t, std::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Find_Random, int64_t, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
BENCHMARK_TEMPLATE(BM_Find_Random, int64_t, int64_t, boost::unordered_map)->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);

BENCHMARK_TEMPLATE(BM_Insert_Erase_Random, int64_t, std::unordered_map)->Arg(1000)->Arg(5000)->Arg(10000)->Arg(25000)->Arg(50000)->Arg(75000)->Arg(100000);
BENCHMARK_TEMPLATE(BM_Insert_Erase_Random, int64_t, absl::flat_hash_map)->Arg(1000)->Arg(5000)->Arg(10000)->Arg(25000)->Arg(50000)->Arg(75000)->Arg(100000);
BENCHMARK_TEMPLATE(BM_Insert_Erase_Random, int64_t, boost::unordered_map)->Arg(1000)->Arg(5000)->Arg(10000)->Arg(25000)->Arg(50000)->Arg(75000)->Arg(100000);


BENCHMARK_MAIN();
