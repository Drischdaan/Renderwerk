#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Threading/ThreadTypes.h"

enum : uint8
{
	DEFAULT_BUFFERED_CONTAINER_BUFFER_COUNT = 2,
};

/**
 * A container that can be used to store items in a thread-safe manner.
 * The container has a fixed number of buffers that can be swapped between.
 * @note Only containers that have an emplace_back method can be used.
 * @tparam TContainer The container type to use.
 * @tparam TItem The item type to store.
 * @tparam BufferCount The number of buffers to use.
 */
template <typename TContainer, typename TItem, size64 BufferCount = DEFAULT_BUFFERED_CONTAINER_BUFFER_COUNT>
class TBufferedContainer
{
public:
	TBufferedContainer() = default;
	~TBufferedContainer() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(TBufferedContainer);

public:
	void Emplace(const TItem& Item)
	{
		FScopedLock Lock(Mutex);
		Buffers[ActiveBufferIndex].emplace_back(Item);
	}

	void Emplace(TItem&& Item)
	{
		FScopedLock Lock(Mutex);
		Buffers[ActiveBufferIndex].emplace_back(std::move(Item));
	}

	void SwapContainers()
	{
		FScopedLock Lock(Mutex);
		ActiveBufferIndex = (ActiveBufferIndex + 1) % BufferCount;
		Buffers.at(ActiveBufferIndex).clear();
	}

	[[nodiscard]] TContainer& GetBackContainer()
	{
		return Buffers.at((ActiveBufferIndex + 1) % BufferCount);
	}

	[[nodiscard]] size64 GetActiveSize()
	{
		FScopedLock Lock(Mutex);
		return Buffers.at(ActiveBufferIndex).size();
	}

private:
	FMutex Mutex;
	uint32 ActiveBufferIndex = 0;
	TArray<TContainer, BufferCount> Buffers;
};
