#pragma once

#include <memory>

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

template <typename T>
using TWeakPtr = std::weak_ptr<T>;

template <typename T>
using TUniquePtr = std::unique_ptr<T>;

template <typename T, typename... TArguments>
TSharedPtr<T> MakeShared(TArguments&&... Arguments)
{
	return std::make_shared<T>(std::forward<TArguments>(Arguments)...);
}

template <typename T, typename... TArguments>
TUniquePtr<T> MakeUnique(TArguments&&... Arguments)
{
	return std::make_unique<T>(std::forward<TArguments>(Arguments)...);
}
