#pragma once

#include <deque>
#include <queue>

#include "mimalloc.h"

#include "Renderwerk/Core/Containers/Vector.hpp"

template <typename T>
using TDequeue = std::deque<T, mi_stl_allocator<T>>;

template <typename T>
using TQueue = std::queue<T, TDequeue<T>>;

template <typename T, typename TContainer = TVector<T>, typename TComparer = std::less<typename TContainer::value_type>>
using TPriorityQueue = std::priority_queue<T, TContainer, TComparer>;
