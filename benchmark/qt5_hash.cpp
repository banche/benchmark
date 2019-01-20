#include "tests.hpp"

#include <QHash>

/// Specialize the adapter for QHash since the interface
/// does not follow most of the existing implementation!
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

DECLARE_ALL_TESTS(QHash)
