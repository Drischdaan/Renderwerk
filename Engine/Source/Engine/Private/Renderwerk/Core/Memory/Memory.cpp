#include "pch.h"

#include "Renderwerk/Core/Memory/Memory.h"

HANDLE FMemory::ProcessHeap = GetProcessHeap();

void* FMemory::Allocate(const size64 Size, const uint8 Alignment)
{
	const size64 AlignedSize = CalculateAlignedSize(Size, Alignment);
	return HeapAlloc(ProcessHeap, 0, AlignedSize);
}

void* FMemory::Reallocate(void* Pointer, const size64 Size, const uint8 Alignment)
{
	return HeapReAlloc(ProcessHeap, 0, Pointer, CalculateAlignedSize(Size, Alignment));
}

void FMemory::Copy(void* Destination, const void* Source, const size64 Size)
{
	CopyMemory(Destination, Source, Size);
}

void FMemory::Free(void* Pointer)
{
	HeapFree(ProcessHeap, 0, Pointer);
}

size64 FMemory::CalculateAlignedSize(const size64 Size, const uint8 Alignment)
{
	return (Size + Alignment - 1) & ~(Alignment - 1);
}

size64 FMemory::GetPointerSize(const void* Memory)
{
	return HeapSize(ProcessHeap, 0, Memory);
}
