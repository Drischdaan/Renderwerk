#include "pch.hpp"

#include "Renderwerk/Core/Memory/Memory.hpp"

#include "mimalloc.h"

void* FMemory::Allocate(const size64 Size, const size64 Alignment)
{
	return mi_malloc_aligned(Size, Alignment);
}

void* FMemory::Reallocate(void* OriginalPointer, const size64 Size, const size64 Alignment)
{
	return mi_realloc_aligned(OriginalPointer, Size, Alignment);
}

void FMemory::Free(void* Pointer, const size64 Alignment)
{
	mi_free_aligned(Pointer, Alignment);
}

bool8 FMemory::IsValidPointer(const void* Pointer)
{
	return mi_check_owned(Pointer);
}

size64 FMemory::GetAllocationSize(const void* Pointer)
{
	return mi_usable_size(Pointer);
}
