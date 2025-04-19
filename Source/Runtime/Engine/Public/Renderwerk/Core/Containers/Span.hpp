#pragma once

#include <span>

#include "Renderwerk/Core/PrimitiveTypes.hpp"

template <typename T, size64 TExtent = std::dynamic_extent>
using TSpan = std::span<T, TExtent>;
