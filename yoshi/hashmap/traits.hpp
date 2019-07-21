#pragma once

#include <type_traits>

template <typename K, typename V, template<typename ...> typename H>
struct Traits
{
    using SupportUnconditionnalRehash = std::false_type;
};
