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
