#pragma once

#include <set>
#include <unordered_set>

#include "mimalloc.h"

template <typename T, typename TComparer = std::less<T>>
using TSet = std::set<T, TComparer, mi_stl_allocator<T>>;

template <typename T, typename THasher = std::hash<T>, typename TComparer = std::less<T>>
using TUnorderedSet = std::unordered_set<T, THasher, TComparer, mi_stl_allocator<T>>;
