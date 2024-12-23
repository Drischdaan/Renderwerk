﻿#pragma once

#include <memory>

#include "Renderwerk/Core/Memory/Memory.h"

template <class T>
struct FDefaultDelete
{
	constexpr FDefaultDelete() noexcept = default;

	template <class TOther>
	constexpr FDefaultDelete(const FDefaultDelete<TOther>&) noexcept requires (std::is_convertible_v<TOther*, T*>)
	{
	}

	constexpr void operator()(const T* Pointer) const noexcept
	{
		FMemory::Delete(const_cast<T*>(Pointer));
	}
};

template <typename T>
using TUniquePtr = std::unique_ptr<T, FDefaultDelete<T>>;

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

template <typename T>
using TWeakPtr = std::weak_ptr<T>;

template <typename T, typename... TArguments>
TUniquePtr<T> MakeUnique(TArguments&&... Arguments) requires (std::is_constructible_v<T, TArguments...>)
{
	T* Pointer = FMemory::New<T>(std::forward<TArguments>(Arguments)...);
	return TUniquePtr<T>(Pointer, FDefaultDelete<T>());
}

template <typename T, typename... TArguments>
TSharedPtr<T> MakeShared(TArguments&&... Arguments) requires (std::is_constructible_v<T, TArguments...>)
{
	T* Pointer = FMemory::New<T>(std::forward<TArguments>(Arguments)...);
	return TSharedPtr<T>(Pointer, FDefaultDelete<T>());
}
