#include "pch.h"

#include "Renderwerk/Platform/Threading/SyncPoint.h"

FString ToString(const ESyncPointState State)
{
	switch (State)
	{
	DEFINE_ENUM_CASE(ESyncPointState, Uninitialized);
	DEFINE_ENUM_CASE(ESyncPointState, Initialized);
	DEFINE_ENUM_CASE(ESyncPointState, Signaled);
	DEFINE_ENUM_CASE(ESyncPointState, Waiting);
	DEFINE_ENUM_CASE(ESyncPointState, Shutdown);
	default:
		return "Unknown";
	}
}

FSyncPoint::FSyncPoint()
	: Semaphore(0), bState(ESyncPointState::Uninitialized), TickCount(0)
{
}

FSyncPoint::~FSyncPoint()
{
}

void FSyncPoint::Wait()
{
	bState = ESyncPointState::Waiting;
	Semaphore.acquire();
	++TickCount;
}

void FSyncPoint::Signal()
{
	Semaphore.release();
	bState = ESyncPointState::Signaled;
}

void FSyncPoint::SetInitializedState()
{
	bState = ESyncPointState::Initialized;
}

void FSyncPoint::SetShutdownState()
{
	bState = ESyncPointState::Shutdown;
}
