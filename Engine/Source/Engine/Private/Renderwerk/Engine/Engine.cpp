#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

FEngine* GEngine = nullptr;

void FEngine::RequestExit()
{
	bStopThreads = true;
}

void FEngine::Initialize()
{
	RW_LOG(LogDefault, Info, "Main thread id: {}", GetCurrentThreadId());
	UpdateThread.Thread = FThread(&FEngine::UpdateThread_Main, this);
	RenderThread.Thread = FThread(&FEngine::RenderThread_Main, this);
}

void FEngine::RunLoop()
{
	while (!bStopThreads)
	{
		MainThread_Tick();
	}
}

void FEngine::Shutdown()
{
	bStopThreads = true;
	RenderThread.SyncPoint.Signal();
	RenderThread.Thread.join();
	UpdateThread.SyncPoint.Signal();
	UpdateThread.Thread.join();
}

void FEngine::MainThread_Tick()
{
	if (GetAsyncKeyState(VK_ESCAPE) & 1)
		RequestExit();
}

void FEngine::UpdateThread_Main()
{
	UpdateThread_Initialize();
	while (!bStopThreads)
	{
		if (RenderThread.SyncPoint.GetState() != ESyncPointState::Initialized)
			continue;
		UpdateThread.SyncPoint.Wait();
		UpdateThread_Tick();
		RenderThread.SyncPoint.Signal();
	}
	UpdateThread_Shutdown();
}

void FEngine::UpdateThread_Initialize()
{
	RW_LOG(LogDefault, Info, "Update thread id: {}", GetCurrentThreadId());
	UpdateThread.SyncPoint.SetInitializedState();
}

void FEngine::UpdateThread_Tick()
{
}

void FEngine::UpdateThread_Shutdown()
{
	UpdateThread.SyncPoint.SetShutdownState();
}

void FEngine::RenderThread_Main()
{
	RenderThread_Initialize();
	while (!bStopThreads)
	{
		RenderThread.SyncPoint.Wait();
		RenderThread_Tick();
		UpdateThread.SyncPoint.Signal();
	}
	RenderThread_Shutdown();
}

void FEngine::RenderThread_Initialize()
{
	RW_LOG(LogDefault, Info, "Render thread id: {}", GetCurrentThreadId());
	RenderThread.SyncPoint.SetInitializedState();
}

void FEngine::RenderThread_Tick()
{
}

void FEngine::RenderThread_Shutdown()
{
	RenderThread.SyncPoint.SetShutdownState();
}

FEngine* GetEngine()
{
	ASSERT_MSG(GEngine, "GEngine is not valid");
	return GEngine;
}
