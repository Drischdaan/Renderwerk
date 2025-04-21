#pragma once

#include <Windows.h>

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Platform/Threading/Lockable.hpp"

class ENGINE_API FResourceLock : public ILockable
{
public:
	FResourceLock();
	~FResourceLock() override;

	DEFAULT_COPY_MOVEABLE(FResourceLock)

public:
	void Lock() override;
	void Unlock() override;

	[[nodiscard]] bool8 TryLock() override;

	void LockRead();
	void UnlockRead();
	[[nodiscard]] bool8 TryLockRead();

	void LockWrite();
	void UnlockWrite();
	[[nodiscard]] bool8 TryLockWrite();

private:
	SRWLOCK ResourceLock;

	friend class ENGINE_API FConditionVariable;
};
