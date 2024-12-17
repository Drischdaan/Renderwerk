#include "pch.h"

#include "Renderwerk/Engine/EngineThreads.h"

#include "Renderwerk/Platform/Window.h"
#include "Renderwerk/Platform/WindowManager.h"
#include "Renderwerk/Renderer/Renderer.h"

IEngineThread::IEngineThread(TAtomic<bool8>* InIsRunning, const FAnsiString& InProfilerName)
	: bIsRunning(InIsRunning), ProfilerName(InProfilerName)
{
	Thread = FThread(&IEngineThread::ThreadEntrypoint, this);
}

IEngineThread::~IEngineThread()
{
	Thread.join();
}

void IEngineThread::Signal()
{
	SyncPoint.Signal();
}

void IEngineThread::Initialize()
{
	PROFILER_SET_THREAD_NAME(ProfilerName.c_str());
	SyncPoint.SetInitializedState();
}

bool8 IEngineThread::PreTick()
{
	SyncPoint.Wait();
	return true;
}

bool8 IEngineThread::Tick()
{
	return true;
}

void IEngineThread::PostTick()
{
}

void IEngineThread::Shutdown()
{
	SyncPoint.SetShutdownState();
}

void IEngineThread::ThreadEntrypoint()
{
	Initialize();
	while (bIsRunning->load())
	{
		PROFILE_SECONDARY_FRAME(ProfilerName.c_str());
		if (!PreTick())
			continue;
		if (!Tick())
			continue;
		PostTick();
	}
	Shutdown();
}

FRenderThread::FRenderThread(TAtomic<bool8>* bIsRunning)
	: IEngineThread(bIsRunning, "RenderThread")
{
}

FRenderThread::~FRenderThread() = default;

void FRenderThread::Initialize()
{
	Window = GetEngine()->GetWindowManager()->Get(GetEngine()->MainWindowGuid);

	FRendererDesc RendererDesc = {};
	RendererDesc.Window = GetEngine()->GetWindowManager()->Get(GetEngine()->MainWindowGuid);
	Renderer = MakeShared<FRenderer>();
	Renderer->Initialize(RendererDesc);
	IEngineThread::Initialize();
}

void FRenderThread::Shutdown()
{
	IEngineThread::Shutdown();
	Renderer->Destroy();
}

bool8 FRenderThread::PreTick()
{
	if (!IEngineThread::PreTick())
		return false;
	if (!Window->IsValid())
		return false;
	ProcessWindowEvents();
	return true;
}

bool8 FRenderThread::Tick()
{
	Renderer->BeginFrame();
	{
	}
	Renderer->EndFrame();
	Window->SignalCloseCondition();
	return true;
}

void FRenderThread::PostTick()
{
	if (GetEngine()->UpdateThread)
		GetEngine()->UpdateThread->Signal();
}

void FRenderThread::ProcessWindowEvents() const
{
	PROFILE_FUNCTION();
	Window->GetEventQueue().SwapContainers();
	TDeque<FWindowEvent> EventQueue = ProcessWindowEventQueue(Window->GetEventQueue().GetBackContainer());
	while (!EventQueue.empty())
	{
		const TSharedPtr<IWindowEvent> Event = EventQueue.front();
		EventQueue.pop_front();
		switch (Event->Type)
		{
		case EWindowEventType::ClientResize:
			{
				Renderer->Resize();
				break;
			}
		default:
			break;
		}
	}
}

FUpdateThread::FUpdateThread(TAtomic<bool8>* bIsRunning)
	: IEngineThread(bIsRunning, "UpdateThread")
{
}

FUpdateThread::~FUpdateThread() = default;

bool8 FUpdateThread::PreTick()
{
	if (GetEngine()->RenderThread->GetSyncPointState() <= ESyncPointState::Initialized)
		return false;
	if (!GetEngine()->GetWindowManager()->IsRegistered(GetEngine()->MainWindowGuid))
		return false;
	return true;
}

void FUpdateThread::PostTick()
{
	GetEngine()->RenderThread->Signal();
	SyncPoint.Wait();
}
