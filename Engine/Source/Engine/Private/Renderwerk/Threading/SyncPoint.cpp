#include "pch.h"

#include "Renderwerk/Threading/SyncPoint.h"

FSyncPoint::FSyncPoint()
	: Semaphore(0), bState(ESyncPointState::Uninitialized), TickCount(0)
{
}

FSyncPoint::~FSyncPoint()
{
}

void FSyncPoint::Wait()
{
	PROFILE_FUNCTION();
	bState = ESyncPointState::Waiting;
	Semaphore.acquire();
	++TickCount;
}

void FSyncPoint::Signal()
{
	PROFILE_FUNCTION();
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
