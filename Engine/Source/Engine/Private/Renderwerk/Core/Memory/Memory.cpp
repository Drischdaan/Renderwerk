#include "pch.h"

#include "Renderwerk/Core/Memory/Memory.h"

HANDLE FMemory::ProcessHeap = GetProcessHeap();

void* FMemory::Allocate(const size64 Size, const size64 Alignment)
{
	VERIFY(Size > 0, "Size must be greater than 0");
	VERIFY(Alignment % 2 == 0, "Alignment must be a multiple of 2");
	const size64 AlignedSize = CalculateAlignedSize(Size, Alignment);
	return HeapAlloc(ProcessHeap, 0, AlignedSize);
}

void* FMemory::Reallocate(void* Pointer, const size64 Size, const size64 Alignment)
{
	VERIFY(Pointer != nullptr, "Pointer must not be nullptr");
	VERIFY(Size > 0, "Size must be greater than 0");
	VERIFY(Alignment % 2 == 0, "Alignment must be a multiple of 2");
	return HeapReAlloc(ProcessHeap, 0, Pointer, CalculateAlignedSize(Size, Alignment));
}

void FMemory::Copy(void* Destination, const void* Source, const size64 Size)
{
	VERIFY(Destination != nullptr, "Destination must not be nullptr");
	VERIFY(Source != nullptr, "Source must not be nullptr");
	VERIFY(Size > 0, "Size must be greater than 0");
	CopyMemory(Destination, Source, Size);
}

void FMemory::Free(void* Pointer)
{
	VERIFY(Pointer != nullptr, "Pointer must not be nullptr");
	HeapFree(ProcessHeap, 0, Pointer);
}

size64 FMemory::CalculateAlignedSize(const size64 Size, const size64 Alignment)
{
	VERIFY(Size > 0, "Size must be greater than 0");
	VERIFY(Alignment % 2 == 0, "Alignment must be a multiple of 2");
	return (Size + Alignment - 1) & ~(Alignment - 1);
}

size64 FMemory::GetPointerSize(const void* Pointer)
{
	VERIFY(Pointer != nullptr, "Pointer must not be nullptr");
	return HeapSize(ProcessHeap, 0, Pointer);
}
