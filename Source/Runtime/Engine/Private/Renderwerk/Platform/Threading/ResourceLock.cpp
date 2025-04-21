#include "pch.hpp"

#include "Renderwerk/Platform/Threading/ResourceLock.hpp"

FResourceLock::FResourceLock()
{
	InitializeSRWLock(&ResourceLock);
}

FResourceLock::~FResourceLock() = default;

void FResourceLock::Lock()
{
	LockWrite();
}

void FResourceLock::Unlock()
{
	UnlockRead();
}

bool8 FResourceLock::TryLock()
{
	return TryLockWrite();
}

void FResourceLock::LockRead()
{
	AcquireSRWLockShared(&ResourceLock);
}

void FResourceLock::UnlockRead()
{
	ReleaseSRWLockShared(&ResourceLock);
}

bool8 FResourceLock::TryLockRead()
{
	return TryAcquireSRWLockShared(&ResourceLock);
}

void FResourceLock::LockWrite()
{
	AcquireSRWLockExclusive(&ResourceLock);
}

void FResourceLock::UnlockWrite()
{
	ReleaseSRWLockExclusive(&ResourceLock);
}

bool8 FResourceLock::TryLockWrite()
{
	return TryAcquireSRWLockExclusive(&ResourceLock);
}
