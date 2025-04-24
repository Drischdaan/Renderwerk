#include "pch.hpp"

#include "Renderwerk/Core/Memory/Memory.hpp"

#include "mimalloc.h"

#include "Renderwerk/Profiler/Profiler.hpp"

void* FMemory::Allocate(const size64 Size, const size64 Alignment)
{
	void* Pointer = mi_malloc_aligned(Size, Alignment);
	PROFILE_POINTER_ALLOCATION(Pointer, Size);
	return Pointer;
}

void* FMemory::Reallocate(void* OriginalPointer, const size64 Size, const size64 Alignment)
{
	PROFILE_POINTER_FREE(OriginalPointer);
	const void* Pointer = mi_realloc_aligned(OriginalPointer, Size, Alignment);
	PROFILE_POINTER_ALLOCATION(Pointer, Size);
	return mi_realloc_aligned(OriginalPointer, Size, Alignment);
}

void FMemory::Free(void* Pointer, const size64 Alignment)
{
	PROFILE_POINTER_FREE(Pointer);
	mi_free_aligned(Pointer, Alignment);
}

void FMemory::Copy(void* Destination, const void* Source, const size64 Size)
{
	memcpy(Destination, Source, Size);
}

void FMemory::Set(void* Pointer, const uint8 Value, const size64 Size)
{
	memset(Pointer, Value, Size);
}

bool8 FMemory::IsValidPointer(const void* Pointer)
{
	return mi_check_owned(Pointer);
}

size64 FMemory::GetAllocationSize(const void* Pointer)
{
	return mi_usable_size(Pointer);
}
