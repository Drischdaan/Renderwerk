#include "pch.hpp"

#include "Renderwerk/Platform/Threading/ScopedLock.hpp"

#include "Renderwerk/Platform/Threading/Lockable.hpp"

FScopedLock::FScopedLock(ILockable* InLockable)
	: Lockable(InLockable)
{
	Lock();
}

FScopedLock::~FScopedLock()
{
	Unlock();
}

void FScopedLock::Lock() const
{
	Lockable->Lock();
}

void FScopedLock::Unlock() const
{
	Lockable->Unlock();
}

bool8 FScopedLock::TryLock() const
{
	return Lockable->TryLock();
}
