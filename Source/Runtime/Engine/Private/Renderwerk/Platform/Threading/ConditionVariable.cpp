#include "pch.hpp"

#include "Renderwerk/Platform/Threading/ConditionVariable.hpp"
#include "Renderwerk/Platform/Threading/CriticalSection.hpp"
#include "Renderwerk/Platform/Threading/ResourceLock.hpp"

FConditionVariable::FConditionVariable()
{
	InitializeConditionVariable(&ConditionVariable);
}

FConditionVariable::~FConditionVariable() = default;

void FConditionVariable::Notify()
{
	WakeConditionVariable(&ConditionVariable);
}

void FConditionVariable::NotifyAll()
{
	WakeAllConditionVariable(&ConditionVariable);
}

bool8 FConditionVariable::Wait(FCriticalSection* Section, const uint64 WaitTime)
{
	return SleepConditionVariableCS(&ConditionVariable, &Section->CriticalSection, static_cast<DWORD>(WaitTime));
}

bool8 FConditionVariable::Wait(FResourceLock* ResourceLock, const uint64 WaitTime)
{
	return SleepConditionVariableSRW(&ConditionVariable, &ResourceLock->ResourceLock, static_cast<DWORD>(WaitTime), 0);
}
