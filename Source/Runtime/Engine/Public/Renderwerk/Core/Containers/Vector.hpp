#pragma once

#include <vector>

#include "mimalloc.h"

template <typename T>
using TVector = std::vector<T, mi_stl_allocator<T>>;
