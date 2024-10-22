#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#define BUFFERED_CONTAINER_SIZE FORWARD(2)

template <typename TContainer, typename TItem>
class FBufferedContainer
{
public:
	FBufferedContainer() = default;
	~FBufferedContainer() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FBufferedContainer);

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
