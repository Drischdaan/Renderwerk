#include "pch.hpp"

#include "Renderwerk/Engine/EngineThread.hpp"

#include "Renderwerk/Core/Misc/StringUtilities.hpp"
#include "Renderwerk/Platform/Threading/ScopedLock.hpp"
#include "Renderwerk/Platform/Threading/Thread.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

IEngineThread::IEngineThread(FString InName, TAtomic<bool8>* InShouldRun, const bool8 bUseExistingThread)
	: Name(std::move(InName)), bShouldRun(InShouldRun)
{
	if (!bUseExistingThread)
	{
		Thread = NewOwned<FThread>([&]([[maybe_unused]] void* UserData)
		{
			const FAnsiString ConvertedName = FStringUtilities::ConvertToAnsi(Name);
			const FAnsiChar* ThreadName = ConvertedName.c_str();
			tracy::SetThreadName(ThreadName);
			Initialize();
			ChangeState(EEngineThreadState::Initialized);
			ChangeState(EEngineThreadState::Running);
			while (bShouldRun->load())
			{
				PROFILE_SECONDARY_FRAME(ThreadName);
				OnTick();
			}
			Shutdown();
			ChangeState(EEngineThreadState::Exited);
			return 0;
		});
		Thread->Start();
	}
}

IEngineThread::~IEngineThread()
{
	if (Thread != nullptr)
	{
		Thread->Join();
	}
}

bool8 IEngineThread::WaitForStateChange()
{
	FScopedLock Lock(&StateChangeSection);
	return StateChangeVariable.Wait(&StateChangeSection);
}

void IEngineThread::WaitForState(const EEngineThreadState StateToWait)
{
	PROFILE_FUNCTION();
	while (static_cast<uint8>(State.load()) < static_cast<uint8>(StateToWait))
	{
		WaitForStateChange();
	}
}

void IEngineThread::ChangeState(const EEngineThreadState NewState)
{
	State = NewState;
	StateChangeVariable.NotifyAll();
}
