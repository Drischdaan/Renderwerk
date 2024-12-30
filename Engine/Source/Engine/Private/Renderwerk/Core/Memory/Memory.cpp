#include "pch.h"

#include "Renderwerk/Core/Memory/Memory.h"

HANDLE FMemory::ProcessHeap = GetProcessHeap();

void* FMemory::Allocate(const size64 Size, const size64 Alignment)
{
	VERIFY(Size > 0, "Size must be greater than 0");
	VERIFY(Alignment % 2 == 0, "Alignment must be a multiple of 2");
	const size64 AlignedSize = CalculateAlignedSize(Size, Alignment);
	void* Pointer = HeapAlloc(ProcessHeap, 0, AlignedSize);
	if (IS_PROFILER_STARTED())
	{
		PROFILER_MARK_ALLOCATION(Pointer, AlignedSize);
	}
	return Pointer;
}

void* FMemory::Reallocate(void* Pointer, const size64 Size, const size64 Alignment)
{
	VERIFY(Pointer != nullptr, "Pointer must not be nullptr");
	VERIFY(Size > 0, "Size must be greater than 0");
	VERIFY(Alignment % 2 == 0, "Alignment must be a multiple of 2");
	if (IS_PROFILER_STARTED())
	{
		PROFILER_MARK_FREE(Pointer);
	}
	void* NewPointer = HeapReAlloc(ProcessHeap, 0, Pointer, CalculateAlignedSize(Size, Alignment));
	if (IS_PROFILER_STARTED())
	{
		PROFILER_MARK_ALLOCATION(NewPointer, Size);
	}
	return NewPointer;
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
	if (Pointer == nullptr)
	{
		RW_LOG(LogDefault, Warning, "Attempted to free nullptr");
		return;
	}
	if (IS_PROFILER_STARTED())
	{
		PROFILER_MARK_FREE(Pointer);
	}
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
