#include "pch.hpp"

#include "Renderwerk/Engine/Threads/RenderThread.hpp"

#include "Renderwerk/Engine/Engine.hpp"

FRenderThread::FRenderThread(TAtomic<bool8>* InShouldRun)
	: IEngineThread(TEXT("Render"), InShouldRun)
{
}

void FRenderThread::Initialize()
{
	GetEngine()->MainThread->WaitForState(EEngineThreadState::Initialized);
	{
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
