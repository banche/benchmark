#include "tests.hpp"
#include "traits.hpp"

#include <absl/container/flat_hash_map.h>


template <typename K, typename V>
struct Traits<K,V, absl::flat_hash_map>
{
    using SupportUnconditionnalRehash = std::true_type;
};

DECLARE_ALL_TESTS(absl::flat_hash_map)
