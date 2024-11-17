#include "pch.h"

#include "Renderwerk/Platform/WindowManager.h"

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

WNDCLASSEX FWindowManager::WindowClass = {};
TMap<FGuid, TSharedPtr<FWindow>> FWindowManager::WindowRegistry = {};
TQueue<FGuid> FWindowManager::DestructionQueue = {};

void FWindowManager::Initialize()
{
	WindowClass.cbSize = sizeof(WNDCLASSEXA);
	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = GetModuleHandle(nullptr);
	WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClass.hbrBackground = nullptr;
	WindowClass.lpszClassName = "RenderwerkWindowClass";
	WindowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	WindowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	RegisterClassEx(&WindowClass);
}

void FWindowManager::Update()
{
	PROFILE_FUNCTION();
	HandleMessages();
	CollectInvalidWindows();
	ProcessDestructionQueue();
}

void FWindowManager::Shutdown()
{
	for (const FGuid& Guid : WindowRegistry | std::views::keys)
		DestroyWindow(Guid);
	ProcessDestructionQueue();
	UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
}

void FWindowManager::HandleMessages()
{
	PROFILE_FUNCTION();
	MSG Message = {};
	while (PeekMessageA(&Message, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessageA(&Message);
	}
}

void FWindowManager::CollectInvalidWindows()
{
	PROFILE_FUNCTION();
	for (const TSharedPtr<FWindow>& Window : WindowRegistry | std::views::values)
	{
		if (Window && !Window->GetState().bIsValid)
			DestroyWindow(Window->GetId());
	}
}

void FWindowManager::ProcessDestructionQueue()
{
	PROFILE_FUNCTION();
	FGuid Guid = FGuid();
	while (!DestructionQueue.empty())
	{
		Guid = DestructionQueue.front();
		DestructionQueue.pop();
		WindowRegistry.erase(Guid);
	}
}

bool8 FWindowManager::IsRegistered(const FGuid& Guid)
{
	return WindowRegistry.contains(Guid);
}

TSharedPtr<FWindow> FWindowManager::NewWindow(const FWindowDesc& Description)
{
	TSharedPtr<FWindow> Window = MakeShared<FWindow>(NewGuid(), Description);
	WindowRegistry.insert({Window->GetId(), Window});
	return Window;
}

TSharedPtr<FWindow> FWindowManager::GetWindow(const FGuid& Guid)
{
	return WindowRegistry.at(Guid);
}

void FWindowManager::DestroyWindow(const FGuid& Guid)
{
	if (IsRegistered(Guid))
		DestructionQueue.push(Guid);
}
