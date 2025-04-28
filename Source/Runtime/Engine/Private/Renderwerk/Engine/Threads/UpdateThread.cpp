#include "pch.hpp"

#include "Renderwerk/Engine/Threads/UpdateThread.hpp"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

FUpdateThread::FUpdateThread(TAtomic<bool8>* InShouldRun)
	: IEngineThread(TEXT("Update"), InShouldRun)
{
}

FUpdateThread::~FUpdateThread() = default;

void FUpdateThread::Initialize()
{
	PROFILE_FUNCTION();

	GetEngine()->MainThread->WaitForState(EEngineThreadState::Initialized);
	{
		PROFILE_SCOPE("InitializeModules");

		const TVector<TRef<IEngineModule>> Modules = GetEngine()->GetModuleListByAffinity(EEngineThreadAffinity::Update);
		for (const TRef<IEngineModule>& EngineModule : Modules)
		{
			EngineModule->Initialize();
		}
	}
	RW_LOG(Info, "Update thread initialized");
}

void FUpdateThread::OnTick()
{
	GetEngine()->GetUpdateThreadTickDelegate().Broadcast();
}

void FUpdateThread::Shutdown()
{
	{
		const TVector<TRef<IEngineModule>> Modules = GetEngine()->GetModuleListByAffinity(EEngineThreadAffinity::Update);
		for (const TRef<IEngineModule>& EngineModule : Modules)
		{
			EngineModule->Shutdown();
		}
	}
	RW_LOG(Info, "Update thread shutdown");
}
