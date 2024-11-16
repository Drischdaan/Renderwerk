#include "pch.h"

#include "Renderwerk/Engine/Engine.h"

LRESULT WindowProc(const HWND WindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
{
	FWindow* Window;
	if (Message == WM_NCCREATE)
	{
		const CREATESTRUCT* CreateStruct = reinterpret_cast<CREATESTRUCT*>(LParam);
		Window = static_cast<FWindow*>(CreateStruct->lpCreateParams);
		SetWindowLongPtr(WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(Window));
	}
	else
	{
		Window = reinterpret_cast<FWindow*>(GetWindowLongPtr(WindowHandle, GWLP_USERDATA));
	}

	if (Window)
		return Window->WindowProc(WindowHandle, Message, WParam, LParam);

	return DefWindowProc(WindowHandle, Message, WParam, LParam);
}

FEngine* GEngine = nullptr;

void FEngine::RequestExit()
{
	bStopThreads = true;
	RW_LOG(LogDefault, Warn, "Engine exit requested");
}

void FEngine::Initialize()
{
	WindowClass.cbSize = sizeof(WNDCLASSEXA);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = GetModuleHandle(nullptr);
	WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClass.hbrBackground = nullptr;
	WindowClass.lpszClassName = "RenderwerkWindowClass";
	WindowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	WindowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	RegisterClassExA(&WindowClass);

	const FWindowDesc WindowDesc = {};
	Window = new FWindow(NewGuid(), WindowDesc);

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

	delete Window;
	Window = nullptr;

	UnregisterClassA(WindowClass.lpszClassName, WindowClass.hInstance);
}

void FEngine::MainThread_Tick()
{
	PROFILE_FUNCTION();
	{
		PROFILE_SCOPE("WindowMessageHandling");
		MSG Message = {};
		while (PeekMessageA(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessageA(&Message);
		}
	}
	if (!Window->GetState().bIsValid)
		RequestExit();

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
		RenderThread_HandleEvents();
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

void FEngine::RenderThread_HandleEvents() const
{
	PROFILE_FUNCTION();
	Window->GetEventQueue().SwapContainers();
	TQueue<FEventPtr>& EventQueue = Window->GetEventQueue().GetBackContainer();
	FEventPtr Event = nullptr;
	while (!EventQueue.empty())
	{
		Event = std::move(EventQueue.front());
		EventQueue.pop();

		switch (Event->Type)
		{
		case EWindowEventType::None:
		default:
			break;
		}
	}
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
