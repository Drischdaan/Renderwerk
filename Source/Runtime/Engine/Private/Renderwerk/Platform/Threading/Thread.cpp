#include "pch.hpp"

#include "Renderwerk/Platform/Threading/Thread.hpp"

DWORD ThreadFunc(const LPVOID ThreadParameter)
{
	const FThread* WindowsThread = static_cast<FThread*>(ThreadParameter);
	if (WindowsThread->Function != nullptr)
	{
		return WindowsThread->Function.operator()();
	}
	return 0;
}

FThread::FThread(FThreadFunc&& InFunction)
	: Function(std::move(InFunction))
{
}

FThread::~FThread()
{
	if (ThreadHandle)
	{
		Join();
		if (ThreadHandle)
		{
			Kill();
		}
	}
}

bool8 FThread::IsValid() const
{
	return ThreadHandle != nullptr;
}

void FThread::Start()
{
	if (State == EThreadState::Running)
		return;
	ThreadHandle = CreateThread(nullptr, 0, ThreadFunc, this, 0, reinterpret_cast<LPDWORD>(&Id));
	State = EThreadState::Running;
}

void FThread::Join(const uint64 WaitTime)
{
	if (State != EThreadState::Running)
		return;
	WaitForSingleObject(ThreadHandle, static_cast<DWORD>(WaitTime));
	CloseHandle(ThreadHandle);
	ThreadHandle = nullptr;
	State = EThreadState::Stopped;
}

void FThread::Kill()
{
	if (State != EThreadState::Running)
		return;
	const DWORD Result = WaitForSingleObject(ThreadHandle, 100);
	if (Result == WAIT_TIMEOUT)
	{
		TerminateThread(ThreadHandle, 0);
	}
	CloseHandle(ThreadHandle);
	ThreadHandle = nullptr;
	State = EThreadState::Stopped;
}

uint64 FThread::GetCurrentId()
{
	return GetCurrentThreadId();
}
