#include "pch.hpp"

#include "Renderwerk/Engine/Threads/RenderThread.hpp"

#include "Renderwerk/Core/Misc/Chrono.hpp"
#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

FRenderThread::FRenderThread(TAtomic<bool8>* InShouldRun)
	: IEngineThread(TEXT("Render"), InShouldRun)
{
}

void FRenderThread::Initialize()
{
	PROFILE_FUNCTION();

	GetEngine()->MainThread->WaitForState(EEngineThreadState::Initialized);
	{
		PROFILE_SCOPE("InitializeModules");

		const TVector<TRef<IEngineModule>> Modules = GetEngine()->GetModuleListByAffinity(EEngineThreadAffinity::Render);
		for (const TRef<IEngineModule>& EngineModule : Modules)
		{
			EngineModule->Initialize();
		}
	}
	RW_LOG(Info, "Render thread initialized");
}

void FRenderThread::OnTick()
{
	PROFILE_FUNCTION();
	GetEngine()->GetRenderThreadTickDelegate().Broadcast();
}

void FRenderThread::Shutdown()
{
	{
		const TVector<TRef<IEngineModule>> Modules = GetEngine()->GetModuleListByAffinity(EEngineThreadAffinity::Render);
		for (const TRef<IEngineModule>& EngineModule : Modules)
		{
			EngineModule->Shutdown();
		}
	}
	RW_LOG(Info, "Render thread shutdown");
}
