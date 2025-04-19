#pragma once

#include <memory>
#include <type_traits>

#include "Renderwerk/Core/Memory/Memory.hpp"

template <typename T>
struct FSmartDeleter
{
	constexpr FSmartDeleter() noexcept = default;

	template <typename TOther> requires std::is_convertible_v<T*, TOther*>
	constexpr FSmartDeleter(const FSmartDeleter<TOther>&) noexcept
	{
	}

	constexpr void operator()(T* Pointer) const noexcept
	{
		std::destroy_at<T>(Pointer);
		FMemory::Free(static_cast<void*>(Pointer));
	}
};

template <typename T>
using TRef = std::shared_ptr<T>;

template <typename T>
using TWeakRef = std::weak_ptr<T>;

template <typename T>
using TOwned = std::unique_ptr<T, FSmartDeleter<T>>;

template <typename T, typename... TArguments> requires std::is_constructible_v<T, TArguments...>
[[nodiscard]] TRef<T> NewRef(TArguments&&... Arguments)
{
	T* Pointer = static_cast<T*>(FMemory::Allocate(sizeof(T)));
	return std::shared_ptr<T>(std::construct_at<T, TArguments...>(Pointer, std::forward<TArguments>(Arguments)...), FSmartDeleter<T>());
}

template <typename T, typename... TArguments> requires std::is_constructible_v<T, TArguments...>
[[nodiscard]] TOwned<T> NewOwned(TArguments&&... Arguments)
{
	T* Pointer = static_cast<T*>(FMemory::Allocate(sizeof(T)));
	return TOwned<T>(std::construct_at<T, TArguments...>(Pointer, std::forward<TArguments>(Arguments)...), FSmartDeleter<T>());
}
