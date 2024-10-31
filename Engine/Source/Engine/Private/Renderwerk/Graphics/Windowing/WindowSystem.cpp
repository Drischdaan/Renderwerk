#include "pch.h"

#include "Renderwerk/Graphics/Windowing/WindowSystem.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Graphics/Windowing/Window.h"

DEFINE_LOG_CATEGORY(LogWindowSystem);

LRESULT WindowProc(const HWND WindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
{
	FWindow* Window;
	if (Message == WM_NCCREATE)
	{
		CREATESTRUCT* CreateStruct = reinterpret_cast<CREATESTRUCT*>(LParam);
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

FWindowSystem::FWindowSystem()
{
}

FWindowSystem::~FWindowSystem()
{
}

void FWindowSystem::Initialize()
{
	WindowClass = {};
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = GetModuleHandle(nullptr);
	WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	WindowClass.lpszClassName = TEXT("RenderwerkWindowClass");
	RegisterClassEx(&WindowClass);

	OnTickHandle = GetEngine()->GetTickDelegate().Bind(BIND_MEMBER_ONE(FWindowSystem::OnTick));

	FWindowDesc MainWindowDesc = {};
	MainWindow = Create(MainWindowDesc);
}

void FWindowSystem::Shutdown()
{
	if (MainWindow->GetState().Visibility != EWindowVisibility::Destroyed && WindowRegistry.contains(MainWindow->GetId()))
		Destroy(MainWindow->GetId());
	GetEngine()->GetTickDelegate().Unbind(OnTickHandle);
	UnregisterClass(WindowClass.lpszClassName, GetModuleHandle(nullptr));
	for (TSharedPtr<FWindow>& Window : WindowRegistry | std::views::values)
		Window.reset();
	WindowRegistry.clear();
}

bool8 FWindowSystem::IsRegistered(const FGuid& InGuid) const
{
	return WindowRegistry.contains(InGuid);
}

TSharedPtr<FWindow> FWindowSystem::Create(const FWindowDesc& InDescription)
{
	TSharedPtr<FWindow> Window = MakeShared<FWindow>(NewGuid(), InDescription);
	Window->AppendTitle(TEXT(" (") + ToString(Window->GetId()) + TEXT(")"));
	WindowRegistry.insert({Window->GetId(), Window});
	RW_LOG(LogWindowSystem, Info, "Created new window: {} [Title: {}, Size:{}x{}]", ToString(Window->GetId()), InDescription.Title, InDescription.Width,
	       InDescription.Height);
	WindowCreatedDelegate.Execute(Window);
	return Window;
}

TSharedPtr<FWindow> FWindowSystem::Get(const FGuid& InGuid) const
{
	DEBUG_ASSERTM(IsRegistered(InGuid), "Window with GUID {} is not registered", ToString(InGuid));
	return WindowRegistry.at(InGuid);
}

TVector<TSharedPtr<FWindow>> FWindowSystem::GetAll() const
{
	TVector<TSharedPtr<FWindow>> Windows;
	for (const TSharedPtr<FWindow>& Window : WindowRegistry | std::views::values)
		Windows.push_back(Window);
	return Windows;
}

void FWindowSystem::Destroy(const FGuid& InGuid)
{
	PROFILER_MARK_FUNCTION();
	TSharedPtr<FWindow> Window = Get(InGuid);
	WindowDestroyedDelegate.Execute(Window);
	WindowRegistry.erase(InGuid);
	RW_LOG(LogWindowSystem, Info, "Destroyed window: {} [Title: {}]", ToString(InGuid), Window->GetState().Title);
	Window.reset();
}

void FWindowSystem::OnTick(MAYBE_UNUSED float64 DeltaTime) const
{
	PROFILER_MARK_FUNCTION();
	MSG Message;
	while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	if (MainWindow->GetState().Visibility == EWindowVisibility::Destroyed)
		GetEngine()->RequestExit();
}
