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

	static void Copy(void* Destination, const void* Source, size64 Size);
	static void Set(void* Pointer, uint8 Value, size64 Size);

public:
	[[nodiscard]] static bool8 IsValidPointer(const void* Pointer);
	[[nodiscard]] static size64 GetAllocationSize(const void* Pointer);
};
