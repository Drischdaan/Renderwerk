#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Platform/Threading/Mutex.h"

#define BUFFERED_CONTAINER_SIZE FORWARD(2)

template <typename TContainer, typename TItem>
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
		ActiveBufferIndex = (ActiveBufferIndex + 1) % BUFFERED_CONTAINER_SIZE;
	}

	NODISCARD TContainer& GetBackContainer()
	{
		return Buffers.at((ActiveBufferIndex + 1) % BUFFERED_CONTAINER_SIZE);
	}

private:
	FMutex Mutex;
	uint32 ActiveBufferIndex = 0;
	TArray<TContainer, 2> Buffers;
};
