#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#define DEFAULT_MEMORY_ALIGNMENT FORWARD(16)

class ENGINE_API FMemory
{
public:
	[[nodiscard]] static void* Allocate(size64 Size, size64 Alignment = DEFAULT_MEMORY_ALIGNMENT);
	[[nodiscard]] static void* Reallocate(void* Memory, size64 Size, size64 Alignment = DEFAULT_MEMORY_ALIGNMENT);
	static void Copy(void* Destination, const void* Source, size64 Size);
	static void Free(void* Memory);

	[[nodiscard]] static size64 CalculateAlignedSize(size64 Size, size64 Alignment = DEFAULT_MEMORY_ALIGNMENT);
};
