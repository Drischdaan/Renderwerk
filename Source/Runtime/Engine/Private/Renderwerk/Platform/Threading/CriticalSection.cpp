#include "pch.hpp"

#include "Renderwerk/Platform/Threading/CriticalSection.hpp"

FCriticalSection::FCriticalSection()
{
	InitializeCriticalSectionAndSpinCount(&CriticalSection, 4000);
}

FCriticalSection::~FCriticalSection()
{
	DeleteCriticalSection(&CriticalSection);
}

void FCriticalSection::Lock()
{
	EnterCriticalSection(&CriticalSection);
}

void FCriticalSection::Unlock()
{
	LeaveCriticalSection(&CriticalSection);
}

bool8 FCriticalSection::TryLock()
{
	return TryEnterCriticalSection(&CriticalSection);
}
