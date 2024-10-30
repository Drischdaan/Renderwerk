#include "pch.h"

#include "Renderwerk/Core/Memory/Memory.h"

#include <Windows.h>

void* FMemory::Allocate(const size64 Size, const size64 Alignment)
{
	DEBUG_ASSERTM(Size > 0, "Size must be greater than zero.");
	DEBUG_ASSERTM(Alignment % 2 == 0, "Alignment must be a power of two.");
	size64 AlignedSize = CalculateAlignedSize(Size, Alignment);
	void* Pointer = HeapAlloc(GetProcessHeap(), 0, AlignedSize);
	if (FProfiler::IsProfilerConnected())
		PROFILER_MARK_ALLOCATION(Pointer, AlignedSize);
	return Pointer;
}

void FMemory::Free(void* Memory)
{
	DEBUG_ASSERTM(Memory != nullptr, "Memory must not be null.");
	if (FProfiler::IsProfilerConnected())
		PROFILER_MARK_FREE(Memory);
	HeapFree(GetProcessHeap(), 0, Memory);
}

void* FMemory::Reallocate(void* Memory, const size64 Size, const size64 Alignment)
{
	DEBUG_ASSERTM(Memory != nullptr, "Memory must not be null.");
	DEBUG_ASSERTM(Size > 0, "Size must be greater than zero.");
	DEBUG_ASSERTM(Alignment % 2 == 0, "Alignment must be a power of two.");
	return HeapReAlloc(GetProcessHeap(), 0, Memory, CalculateAlignedSize(Size, Alignment));
}

void FMemory::Copy(void* Destination, const void* Source, const size64 Size)
{
	DEBUG_ASSERTM(Destination != nullptr, "Destination must not be null.");
	DEBUG_ASSERTM(Source != nullptr, "Source must not be null.");
	DEBUG_ASSERTM(Size > 0, "Size must be greater than zero.");
	CopyMemory(Destination, Source, Size);
}

size64 FMemory::GetSizeOf(const void* Memory)
{
	DEBUG_ASSERTM(Memory != nullptr, "Memory must not be null.");
	return HeapSize(GetProcessHeap(), 0, Memory);
}

size64 FMemory::CalculateAlignedSize(const size64 Size, const size64 Alignment)
{
	DEBUG_ASSERTM(Size > 0, "Size must be greater than zero.");
	DEBUG_ASSERTM(Alignment % 2 == 0, "Alignment must be a power of two.");
	return (Size + Alignment - 1) & ~(Alignment - 1);
}
