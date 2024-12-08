#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Threading/ThreadTypes.h"

enum class ENGINE_API ESyncPointState : uint8
{
	Uninitialized = 0,
	Initialized,
	Signaled,
	Waiting,
	Shutdown,
};

class ENGINE_API FSyncPoint
{
public:
	FSyncPoint();
	~FSyncPoint();

	DEFINE_DEFAULT_COPY_AND_MOVE(FSyncPoint);

public:
	/**
	 * Wait for the synchronization point to be signaled.
	 */
	void Wait();

	/**
	 * Signal the synchronization point, so that the waiting thread can continue.
	 * @note You shouldn't call this function from the waiting thread.
	 */
	void Signal();

	void SetInitializedState();
	void SetShutdownState();

public:
	[[nodiscard]] ESyncPointState GetState() const { return bState; }
	[[nodiscard]] uint64 GetTickCount() const { return TickCount; }

private:
	FBinarySemaphore Semaphore;
	TAtomic<ESyncPointState> bState;
	TAtomic<uint64> TickCount;
};
