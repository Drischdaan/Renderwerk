#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"

class ILockable;

class ENGINE_API FScopedLock
{
public:
	explicit FScopedLock(ILockable* InLockable);
	~FScopedLock();

	NON_COPY_MOVEABLE(FScopedLock)

public:
	void Lock() const;
	void Unlock() const;

	[[nodiscard]] bool8 TryLock() const;

private:
	ILockable* Lockable;
};
