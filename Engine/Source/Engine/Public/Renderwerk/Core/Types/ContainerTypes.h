#pragma once

#include <array>
#include <deque>
#include <initializer_list>
#include <map>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

template <typename T, size_t TSize>
using TArray = std::array<T, TSize>;

template <typename T>
using TVector = std::vector<T>;

template <typename T>
using TSet = std::set<T>;

template <typename TKey, typename TValue>
using TMap = std::map<TKey, TValue>;

template <typename TKey, typename TValue>
using TUnorderedMap = std::unordered_map<TKey, TValue>;

template <typename T>
using TUnorderedSet = std::unordered_set<T>;

template <typename T>
using TDeque = std::deque<T>;

template <typename T>
using TQueue = std::queue<T>;

template <typename T>
using TInitializerList = std::initializer_list<T>;
