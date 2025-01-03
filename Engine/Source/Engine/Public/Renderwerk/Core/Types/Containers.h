﻿#pragma once

#include <array>
#include <deque>
#include <map>
#include <queue>
#include <set>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Renderwerk/Core/CoreTypes.h"
#include "Renderwerk/Core/Memory/Allocators.h"

template <typename T, size64 TSize>
using TArray = std::array<T, TSize>;

template <typename T>
using TVector = std::vector<T, FSTLAllocator<T>>;

template <typename TKey, typename TValue>
using TMap = std::map<TKey, TValue, std::less<TKey>, FSTLAllocator<std::pair<const TKey, TValue>>>;

template <typename TKey, typename TValue>
using TUnorderedMap = std::unordered_map<TKey, TValue, std::hash<TKey>, std::equal_to<TKey>, FSTLAllocator<std::pair<const TKey, TValue>>>;

template <typename T>
using TSet = std::set<T, std::less<T>, FSTLAllocator<T>>;

template <typename T>
using TUnorderedSet = std::unordered_set<T, FSTLAllocator<T>>;

template <typename T>
using TDeque = std::deque<T, FSTLAllocator<T>>;

template <typename T>
using TQueue = std::queue<T, TDeque<T>>;

template <typename T, size64 TExtent = std::dynamic_extent>
using TSpan = std::span<T, TExtent>;
