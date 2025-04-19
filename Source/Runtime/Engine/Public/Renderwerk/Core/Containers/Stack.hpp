#pragma once

#include <stack>

#include "Renderwerk/Core/Containers/Queue.hpp"

template <typename T>
using TStack = std::stack<T, TDequeue<T>>;
