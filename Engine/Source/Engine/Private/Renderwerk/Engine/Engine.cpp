#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

FEngine* GEngine = nullptr;

void FEngine::RequestExit()
{
	bStopThreads = true;
	RW_LOG(LogDefault, Warn, "Engine exit requested");
}

void FEngine::Initialize()
{
	RW_LOG(LogDefault, Info, "Main thread id: {}", GetCurrentThreadId());
	PROFILER_SET_THREAD_NAME("MainThread");
	UpdateThread.Thread = FThread(&FEngine::UpdateThread_Main, this);
	RenderThread.Thread = FThread(&FEngine::RenderThread_Main, this);
}

void FEngine::RunLoop()
{
	while (!bStopThreads)
	{
		PROFILE_FRAME();
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
	PROFILE_FUNCTION();
	if (GetAsyncKeyState(VK_ESCAPE) & 1)
		RequestExit();
}

void FEngine::UpdateThread_Main()
{
	PROFILER_SET_THREAD_NAME("UpdateThread");
	UpdateThread_Initialize();
	while (!bStopThreads)
	{
		PROFILE_SECONDARY_FRAME("Update");
		PROFILE_SCOPE("UpdateThread_Loop");
		if (RenderThread.SyncPoint.GetState() <= ESyncPointState::Initialized)
			continue;
		UpdateThread_Tick();
		RenderThread.SyncPoint.Signal();
		UpdateThread.SyncPoint.Wait();
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
	PROFILE_FUNCTION();
}

void FEngine::UpdateThread_Shutdown()
{
	UpdateThread.SyncPoint.SetShutdownState();
}

void FEngine::RenderThread_Main()
{
	PROFILER_SET_THREAD_NAME("RenderThread");
	RenderThread_Initialize();
	while (!bStopThreads)
	{
		PROFILE_SECONDARY_FRAME("Render");
		PROFILE_SCOPE("RenderThread_Loop");
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
	PROFILE_FUNCTION();
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
