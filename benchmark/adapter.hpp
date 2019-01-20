#pragma once
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
