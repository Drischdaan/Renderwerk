#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"

#ifndef RW_DEFAULT_ALIGNMENT
#	define RW_DEFAULT_ALIGNMENT 8
#endif

class ENGINE_API FMemory
{
public:
	static void* Allocate(size64 Size, size64 Alignment = RW_DEFAULT_ALIGNMENT);
	static void* Reallocate(void* OriginalPointer, size64 Size, size64 Alignment = RW_DEFAULT_ALIGNMENT);
	static void Free(void* Pointer, size64 Alignment = RW_DEFAULT_ALIGNMENT);

public:
	[[nodiscard]] static bool8 IsValidPointer(const void* Pointer);
	[[nodiscard]] static size64 GetAllocationSize(const void* Pointer);
};
