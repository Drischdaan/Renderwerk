#include "pch.hpp"

#include "Renderwerk/Platform/Threading/Mutex.hpp"

FMutex::FMutex()
{
	MutexHandle = CreateMutex(nullptr, false, nullptr);
}

FMutex::~FMutex()
{
	CloseHandle(MutexHandle);
}

void FMutex::Lock()
{
	RW_VERIFY_MSG(WaitForSingleObject(MutexHandle, INFINITE) == WAIT_OBJECT_0, "Unable to wait for mutex handle");
}

void FMutex::Unlock()
{
	ReleaseMutex(MutexHandle);
}

bool8 FMutex::TryLock()
{
	return WaitForSingleObject(MutexHandle, 0) == WAIT_OBJECT_0;
}
