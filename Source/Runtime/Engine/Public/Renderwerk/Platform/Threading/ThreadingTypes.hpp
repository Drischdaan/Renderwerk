#pragma once

#include <future>
#include <variant>

template <typename T>
using TPackagedTask = std::packaged_task<T>;

template <typename T>
using TFuture = std::future<T>;

template <typename... TArguments>
using TVariant = std::variant<TArguments...>;

using FMonoState = std::monostate;
