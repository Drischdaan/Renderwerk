#pragma once

#include <Windows.h>

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Platform/Threading/Lockable.hpp"

class ENGINE_API FMutex : public ILockable
{
public:
	FMutex();
	~FMutex() override;

	DEFAULT_COPY_MOVEABLE(FMutex)

public:
	void Lock() override;
	void Unlock() override;

	[[nodiscard]] bool8 TryLock() override;

private:
	HANDLE MutexHandle;
};
