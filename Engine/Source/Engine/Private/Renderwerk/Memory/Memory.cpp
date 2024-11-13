#include "pch.h"

#include "Renderwerk/Memory/Memory.h"

#include <Windows.h>

void* FMemory::Allocate(const size64 Size, const size64 Alignment)
{
	const size64 AlignedSize = CalculateAlignedSize(Size, Alignment);
	return HeapAlloc(GetProcessHeap(), 0, AlignedSize);
}

void* FMemory::Reallocate(void* Memory, const size64 Size, const size64 Alignment)
{
	return HeapReAlloc(GetProcessHeap(), 0, Memory, CalculateAlignedSize(Size, Alignment));
}

void FMemory::Copy(void* Destination, const void* Source, const size64 Size)
{
	CopyMemory(Destination, Source, Size);
}

void FMemory::Free(void* Memory)
{
	HeapFree(GetProcessHeap(), 0, Memory);
}

size64 FMemory::CalculateAlignedSize(const size64 Size, const size64 Alignment)
{
	return (Size + Alignment - 1) & ~(Alignment - 1);
}

size64 FMemory::GetMemorySize(const void* Memory)
{
	return HeapSize(GetProcessHeap(), 0, Memory);
}
