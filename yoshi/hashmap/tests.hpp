#pragma once

#include "adapter.hpp"
#include "yoshi/yoshi.hpp"

#include <algorithm>
#include <random>

/// Inserts [0, state.range(0) -1] in sequential order
template <typename K, typename V, template<typename ...> typename H>
void Insert_Sequential(benchmark::State& state)
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
static void Insert_Random(benchmark::State& state)
{
    using AdapterT = Adapter<K, V, H>;
    using Type = typename AdapterT::C;
    Type c;
    auto value = ValueSelector<V>::value();
    std::vector<K> keys;
    keys.reserve(state.range(0));
    const std::int64_t SEED = 0;
    std::mt19937_64 generator(SEED);
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
static void Erase_Sequential(benchmark::State& state)
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
static void Erase_Random(benchmark::State& state)
{
    using AdapterT = Adapter<K, V, H>;
    using Type = typename AdapterT::C;
    Type c;
    auto value = ValueSelector<V>::value();
    std::vector<K> keys;
    keys.reserve(state.range(0));
    const std::int64_t SEED = 0;
    std::mt19937_64 generator(SEED);
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
static void Find_Sequential(benchmark::State& state)
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
static void Find_Random(benchmark::State& state)
{
    using AdapterT = Adapter<K, V, H>;
    using Type = typename AdapterT::C;
    Type c;
    auto value = ValueSelector<V>::value();
    std::vector<K> keys;
    keys.reserve(state.range(0));
    const std::int64_t SEED = 0;
    std::mt19937_64 generator(SEED);
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
    const std::int64_t SEED = 0;
    std::mt19937_64 generator(SEED);
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
static void Insert_Erase_Random(benchmark::State& state)
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

template <typename K, typename V, template<typename ...> typename H>
void Find_HalfHit(benchmark::State& state)
{
    using AdapterT = Adapter<K, V, H>;
    using Type = typename AdapterT::C;
    Type c;
    const auto value = ValueSelector<V>::value();

    std::vector<K> keys;
    keys.reserve(state.range(0));
    const std::int64_t SEED = 0;
    std::mt19937_64 generator(SEED);

    for(auto _: state)
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
        int64_t inserted = 0;
        for (K i = 0; i < state.range(0); i += 2)
        {
            auto key = keys[i];
            AdapterT::insert(c, key, value);
            inserted++;
        }

        state.ResumeTiming();
        int64_t found = 0;
        for (K i = 0; i < state.range(0); ++i)
        {
            auto key = keys[i];
            auto it = AdapterT::find(c, key);
            found += (it != AdapterT::end(c));
        }
        state.PauseTiming();
        if (found != inserted)
        {
            std::string error = std::string("excepted ") + std::to_string(inserted);
            error += std::string("got ") + std::to_string(found);
            throw std::runtime_error(error);
        }
        state.ResumeTiming();
    }
}

template <typename K, typename V, template<typename ...> typename H>
void Find_Miss(benchmark::State& state)
{
    using AdapterT = Adapter<K, V, H>;
    using Type = typename AdapterT::C;
    Type c;
    const auto value = ValueSelector<V>::value();

    std::vector<K> keys;
    std::vector<K> missing;
    keys.reserve(state.range(0));
    missing.reserve(state.range(0));
    const std::int64_t SEED = 0;
    std::mt19937_64 generator(SEED);

    for(auto _: state)
    {
        state.PauseTiming();
        keys.clear();
        missing.clear();
        for(K i = 0; i < state.range(0); ++i)
        {
            keys.push_back(i);
        }
        std::shuffle(keys.begin(), keys.end(), generator);
        AdapterT::clear(c);
        AdapterT::reserve(c, state.range(0));
        int64_t inserted = 0;
        for (K i = 0; i < state.range(0); i++)
        {
            const bool isEven = (i%2 == 0);
            if (isEven)
            {
                auto key = keys[i];
                AdapterT::insert(c, key, value);
                inserted++;
            }
            else
            {
                missing.push_back(keys[i]);
            }
        }
        state.ResumeTiming();
        int64_t found = 0;
        for (auto k: missing)
        {
            auto it = AdapterT::find(c, k);
            found += (it != AdapterT::end(c));
        }
        state.PauseTiming();
        if (found != 0)
        {
            throw std::runtime_error("should not be able to find any element");
        }
        state.ResumeTiming();
    }
}

#define DECLARE_ALL_TESTS(C) \
    YOSHI_ADD_BENCHMARK(Insert_Sequential, int64_t, int64_t, C) \
    YOSHI_ADD_BENCHMARK(Insert_Sequential, int32_t, int32_t, C) \
    YOSHI_ADD_BENCHMARK(Insert_Random, int64_t, int64_t, C)     \
    YOSHI_ADD_BENCHMARK(Insert_Random, int64_t, std::string, C) \
    YOSHI_ADD_BENCHMARK(Erase_Sequential, int64_t, int64_t, C)  \
    YOSHI_ADD_BENCHMARK(Erase_Random, int64_t, int64_t, C)      \
    YOSHI_ADD_BENCHMARK(Find_Sequential, int64_t, int64_t, C)   \
    YOSHI_ADD_BENCHMARK(Find_Random, int64_t, int64_t, C)       \
    YOSHI_ADD_SHORT_BENCHMARK(Insert_Erase_Random, int64_t, C)  \
    YOSHI_ADD_BENCHMARK(Find_HalfHit, int64_t, int64_t, C)      \
    YOSHI_ADD_BENCHMARK(Find_Miss, int64_t, int64_t, C)
