#pragma once

#include <Windows.h>

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Platform/Threading/Lockable.hpp"

class ENGINE_API FCriticalSection : public ILockable
{
public:
	FCriticalSection();
	~FCriticalSection() override;

	DEFAULT_COPY_MOVEABLE(FCriticalSection)

public:
	void Lock() override;
	void Unlock() override;

	[[nodiscard]] bool8 TryLock() override;

private:
	CRITICAL_SECTION CriticalSection;

	friend class FConditionVariable;
};
