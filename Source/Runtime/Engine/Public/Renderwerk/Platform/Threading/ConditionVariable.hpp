#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"

class FResourceLock;
class FCriticalSection;

class ENGINE_API FConditionVariable
{
public:
	FConditionVariable();
	~FConditionVariable();

	DEFAULT_COPY_MOVEABLE(FConditionVariable)

public:
	void Notify();
	void NotifyAll();

	/**
	 * Waits for a notification on this condition variable.
	 *
	 * @param Section A pointer to a critical section that MUST be locked before calling this function.
	 *                This critical section will be automatically released while waiting and
	 *                reacquired when the function returns.
	 * @param WaitTime Maximum time to wait in milliseconds, or INFINITE to wait indefinitely.
	 *                 Default is INFINITE.
	 * @return true if the wait was satisfied by a notification, false if the wait timed out.
	 *
	 * @note The critical section must have been entered (locked) before calling this function.
	 * @note This function atomically releases the critical section.
	 */
	[[nodiscard]] bool8 Wait(FCriticalSection* Section, uint64 WaitTime = INFINITE);

	[[nodiscard]] bool8 Wait(FResourceLock* ResourceLock, uint64 WaitTime = INFINITE);

private:
	CONDITION_VARIABLE ConditionVariable;
};
