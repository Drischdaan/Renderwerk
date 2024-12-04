#pragma once

#include "Renderwerk/Core/CoreAPI.h"
#include "Renderwerk/Core/CoreTypes.h"

#include <Windows.h>

enum : uint8
{
	DEFAULT_MEMORY_ALIGNMENT = 16
};

class ENGINE_API FMemory
{
public:
	[[nodiscard]] static void* Allocate(size64 Size, uint8 Alignment = DEFAULT_MEMORY_ALIGNMENT);
	[[nodiscard]] static void* Reallocate(void* Pointer, size64 Size, uint8 Alignment = DEFAULT_MEMORY_ALIGNMENT);
	static void Copy(void* Destination, const void* Source, size64 Size);
	static void Free(void* Pointer);

	[[nodiscard]] static size64 CalculateAlignedSize(size64 Size, uint8 Alignment = DEFAULT_MEMORY_ALIGNMENT);
	[[nodiscard]] static size64 GetPointerSize(const void* Memory);

public:
	template <typename T, typename... TArguments>
	[[nodiscard]] static T* NewAligned(size64 Alignment, TArguments&&... Arguments) requires (std::is_constructible_v<T, TArguments...>);

	template <typename T, typename... TArguments>
	[[nodiscard]] static T* New(TArguments&&... Arguments) requires (std::is_constructible_v<T, TArguments...>);

	template <typename T>
	static void Delete(T* Object);

	template <typename T, typename... TArguments>
	[[nodiscard]] static T* NewArrayAligned(size64 Count, size64 Alignment, TArguments&&... Arguments) requires (std::is_constructible_v<T, TArguments...>);

	template <typename T, typename... TArguments>
	[[nodiscard]] static T* NewArray(size64 Count, TArguments&&... Arguments) requires (std::is_constructible_v<T, TArguments...>);

	template <typename T>
	static void DeleteArray(T* Array, size64 Count);

	static size64 Usage;

private:
	static HANDLE ProcessHeap;
};

template <typename T, typename... TArguments>
T* FMemory::NewAligned(const size64 Alignment, TArguments&&... Arguments) requires (std::is_constructible_v<T, TArguments...>)
{
	void* Memory = Allocate(sizeof(T), Alignment);
	return new(Memory) T(std::forward<TArguments>(Arguments)...);
}

template <typename T, typename... TArguments>
T* FMemory::New(TArguments&&... Arguments) requires (std::is_constructible_v<T, TArguments...>)
{
	return NewAligned<T>(DEFAULT_MEMORY_ALIGNMENT, std::forward<TArguments>(Arguments)...);
}

template <typename T>
void FMemory::Delete(T* Object)
{
	Object->~T();
	Free(Object);
}

template <typename T, typename... TArguments>
T* FMemory::NewArrayAligned(const size64 Count, const size64 Alignment, TArguments&&... Arguments) requires (std::is_constructible_v<T, TArguments...>)
{
	const size64 Size = sizeof(T) * Count;
	void* Memory = Allocate(Size, Alignment);
	T* Array = static_cast<T*>(Memory);
	for (size64 Index = 0; Index < Count; ++Index)
		new(Array + Index) T(std::forward<TArguments>(Arguments)...);
	return Array;
}

template <typename T, typename... TArguments>
T* FMemory::NewArray(const size64 Count, TArguments&&... Arguments) requires (std::is_constructible_v<T, TArguments...>)
{
	return NewArrayAligned<T>(Count, DEFAULT_MEMORY_ALIGNMENT, std::forward<TArguments>(Arguments)...);
}

template <typename T>
void FMemory::DeleteArray(T* Array, const size64 Count)
{
	for (size64 Index = 0; Index < Count; ++Index)
		std::destroy_at(Array + Index);
	Free(Array);
}
