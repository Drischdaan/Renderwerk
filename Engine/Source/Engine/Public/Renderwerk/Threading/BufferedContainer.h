#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Threading/ThreadTypes.h"

enum : uint8
{
	DEFAULT_BUFFERED_CONTAINER_BUFFER_COUNT = 2
};

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
		Buffers[ActiveBufferIndex].emplace(Item);
	}

	void Emplace(TItem&& Item)
	{
		FScopedLock Lock(Mutex);
		Buffers[ActiveBufferIndex].emplace(std::move(Item));
	}

	void SwapContainers()
	{
		FScopedLock Lock(Mutex);
		ActiveBufferIndex = (ActiveBufferIndex + 1) % BufferCount;
	}

	[[nodiscard]] TContainer& GetBackContainer()
	{
		return Buffers.at((ActiveBufferIndex + 1) % BufferCount);
	}

private:
	FMutex Mutex;
	uint32 ActiveBufferIndex = 0;
	TArray<TContainer, BufferCount> Buffers;
};
