#include "tests.hpp"

#include "tsl/robin_map.h"
#include <map>

// use a type alias else google::benchmark is having errors
// because the name is getting too long!
template<typename K, typename V>
using tsl_robin_map = tsl::robin_map<K,V>;

DECLARE_ALL_TESTS(tsl_robin_map)
