#pragma once

#include <iostream>
#include <type_traits>

#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Memory/Memory.hpp"

template <typename T>
class TPoolAllocator
{
private:
	template <typename TObject>
	struct TAlignedStorage
	{
		alignas(alignof(TObject)) std::byte Data[sizeof(TObject)];
	};

	struct FBlock
	{
		FBlock* Next = nullptr;
		TAlignedStorage<T> Storage;
	};

public:
	TPoolAllocator(const size64 InCapacity)
		: Capacity(InCapacity)
	{
		MemoryBlock = static_cast<FBlock*>(FMemory::Allocate(sizeof(FBlock) * Capacity));
		NextFreeBlock = &MemoryBlock[0];
		for (size64 Index = 0; Index < Capacity - 1; ++Index)
		{
			MemoryBlock[Index].Next = &MemoryBlock[Index + 1];
		}
		MemoryBlock[Capacity - 1].Next = nullptr;
	}

	~TPoolAllocator()
	{
		Destroy();
	}

	NON_COPYABLE(TPoolAllocator)
	DEFAULT_MOVEABLE(TPoolAllocator)

public:
	[[nodiscard]] T* Allocate()
	{
		FBlock* Block = NextFreeBlock;
		NextFreeBlock = Block->Next;
		++Usage;
		return reinterpret_cast<T*>(&Block->Storage);
	}

	template <typename... TArguments> requires std::is_constructible_v<T, TArguments...>
	[[nodiscard]] T* NewInstance(TArguments&&... Arguments)
	{
		T* Pointer = Allocate();
		return std::construct_at<T, TArguments...>(Pointer, std::forward<TArguments>(Arguments)...);
	}

	void Free(T* Pointer)
	{
		if (!Pointer)
		{
			return;
		}
		FBlock* PointerBlock = reinterpret_cast<FBlock*>(reinterpret_cast<char*>(Pointer) - offsetof(FBlock, Storage));
		PointerBlock->Next = NextFreeBlock;
		NextFreeBlock = PointerBlock;
		--Usage;
	}

	void DestroyInstance(T* Pointer)
	{
		if (!Pointer)
		{
			return;
		}
		std::destroy_at<T>(Pointer);
		Free(Pointer);
	}

	void Destroy()
	{
		if (!MemoryBlock)
		{
			return;
		}
		FMemory::Free(static_cast<void*>(MemoryBlock));
		MemoryBlock = nullptr;
	}

	[[nodiscard]] bool8 Contains(const T* Pointer) const
	{
		if (!Pointer || !MemoryBlock)
		{
			return false;
		}
		const std::byte* PoolStart = reinterpret_cast<const std::byte*>(MemoryBlock);
		const std::byte* PoolEnd = PoolStart + (sizeof(FBlock) * Capacity);

		const FBlock* PointerBlock = reinterpret_cast<const FBlock*>(reinterpret_cast<const std::byte*>(Pointer) - offsetof(FBlock, Storage));
		const std::byte* PointerAsByte = reinterpret_cast<const std::byte*>(PointerBlock);

		if (PointerAsByte >= PoolStart && PointerAsByte < PoolEnd)
		{
			const uintptr_t Offset = PointerAsByte - PoolStart;
			return (Offset % sizeof(FBlock)) == 0;
		}
		return false;
	}

public:
	[[nodiscard]] size64 GetCapacity() const { return Capacity; }
	[[nodiscard]] size64 GetUsage() const { return Usage; }

private:
	size64 Capacity;

	FBlock* MemoryBlock;
	FBlock* NextFreeBlock;

	size64 Usage = 0;
};
