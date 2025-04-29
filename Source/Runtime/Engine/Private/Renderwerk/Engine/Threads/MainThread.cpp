#include "pch.hpp"

#include "Renderwerk/Engine/Threads/MainThread.hpp"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Engine/EngineModule.hpp"
#include "Renderwerk/Platform/Window.hpp"
#include "Renderwerk/Platform/WindowModule.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

FMainThread::FMainThread(TAtomic<bool8>* InShouldRun)
	: IEngineThread(TEXT("Main"), InShouldRun, true)
{
}

void FMainThread::Initialize()
{
	PROFILE_FUNCTION();

	PROFILER_SET_THREAD_NAME("Main");
	{
		const TRef<FEngine> Engine = GetEngine();
		{
			PROFILE_SCOPE("InitializeModules");

			const TVector<TRef<IEngineModule>> Modules = Engine->GetModuleListByAffinity(EEngineThreadAffinity::Main);
			for (const TRef<IEngineModule>& EngineModule : Modules)
			{
				EngineModule->Initialize();
			}
		}
		const TRef<FWindowModule> WindowModule = Engine->GetModule<FWindowModule>();

		FWindowDesc Description = {};
		Description.Title = FStringUtilities::Format(TEXT("{} v{} | {}"), TEXT(RW_ENGINE_NAME), TEXT(RW_ENGINE_FULL_VERSION), TEXT(RW_CONFIG));
		Window = WindowModule->NewWindow(Description);
		Window->Show();
	}
	RW_LOG(Info, "Main thread initialized");
	ChangeState(EEngineThreadState::Initialized);
	ChangeState(EEngineThreadState::Running);
	GetEngine()->UpdateThread->WaitForState(EEngineThreadState::Initialized);
	GetEngine()->RenderThread->WaitForState(EEngineThreadState::Initialized);
}

void FMainThread::OnTick()
{
	const TRef<FEngine> Engine = GetEngine();
	Engine->GetMainThreadTickDelegate().Broadcast();

	const FWindowState WindowState = Window->GetState();
	if (WindowState.bIsClosed || WindowState.bIsDestroyed)
	{
		Engine->RequestShutdown();
	}
}

void FMainThread::Shutdown()
{
	const TRef<FEngine> Engine = GetEngine();
	Engine->RenderThread->WaitForState(EEngineThreadState::Exited);
	Engine->UpdateThread->WaitForState(EEngineThreadState::Exited);
	{
		{
			const TRef<FWindowModule> WindowModule = Engine->GetModule<FWindowModule>();
			WindowModule->DestroyWindow(std::move(Window));
		}
		const TVector<TRef<IEngineModule>> Modules = Engine->GetModuleListByAffinity(EEngineThreadAffinity::Main);
		for (const TRef<IEngineModule>& EngineModule : Modules)
		{
			EngineModule->Shutdown();
		}
	}
	RW_LOG(Info, "Main thread shutdown");
	ChangeState(EEngineThreadState::Exited);
}
