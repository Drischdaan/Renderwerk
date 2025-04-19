#pragma once

#include <map>
#include <unordered_map>

#include "mimalloc.h"

template <typename TKey, typename TValue, typename TComparer = std::less<TKey>>
using TMap = std::map<TKey, TValue, TComparer, mi_stl_allocator<std::pair<const TKey, TValue>>>;

template <typename TKey, typename TValue, typename THash = std::hash<TKey>, typename TComparer = std::equal_to<TKey>>
using TUnorderedMap = std::unordered_map<TKey, TValue, THash, TComparer, mi_stl_allocator<std::pair<const TKey, TValue>>>;
