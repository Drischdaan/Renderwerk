#include "pch.h"

#include "Renderwerk/Platform/WindowManager.h"

#include "Renderwerk/Platform/Window.h"

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

FWindowManager::FWindowManager()
{
	WindowClass = {};
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_OWNDC;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = GetModuleHandle(nullptr);
	WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	WindowClass.lpszClassName = TEXT("RenderwerkWindowClass");
	VERIFY(RegisterClassEx(&WindowClass), "Failed to register window class");
}

FWindowManager::~FWindowManager()
{
	ProcessInvalidWindows();
	Windows.clear();
	VERIFY(UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance), "Failed to unregister window class");
}

FGuid FWindowManager::Create(const FWindowDesc& Description)
{
	FScopedLock Lock(Mutex);
	FGuid Id;
	Windows.insert_or_assign(Id, MakeShared<FWindow>(Id, Description));
	return Id;
}

TSharedPtr<FWindow> FWindowManager::CreateAndGet(const FWindowDesc& Description)
{
	FScopedLock Lock(Mutex);
	FGuid Id;
	TSharedPtr<FWindow> Window = MakeShared<FWindow>(Id, Description);
	Windows.insert_or_assign(Id, Window);
	return Window;
}

bool8 FWindowManager::IsRegistered(const FGuid& Id) const
{
	return Windows.contains(Id);
}

TSharedPtr<FWindow> FWindowManager::Get(const FGuid& Id)
{
	FScopedLock Lock(Mutex);
	VERIFY(IsRegistered(Id), "window not registered");
	return Windows.at(Id);
}

void FWindowManager::Destroy(const FGuid& Id)
{
	FScopedLock Lock(Mutex);
	InvalidWindowQueue.push(Id);
}

void FWindowManager::Destroy(const TSharedPtr<FWindow>& Window)
{
	FScopedLock Lock(Mutex);
	InvalidWindowQueue.push(Window->GetId());
}

void FWindowManager::ProcessMessages()
{
	PROFILE_FUNCTION();
	ProcessInvalidWindows();

	MSG Message;
	while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
}

void FWindowManager::ProcessInvalidWindows()
{
	PROFILE_FUNCTION();
	FScopedLock Lock(Mutex);
	for (const TSharedPtr<FWindow>& Window : Windows | std::views::values)
	{
		if (!Window->IsValid())
			InvalidWindowQueue.push(Window->GetId());
	}
	if (InvalidWindowQueue.empty())
		return;
	while (!InvalidWindowQueue.empty())
	{
		FGuid Id = InvalidWindowQueue.front();
		InvalidWindowQueue.pop();
		TSharedPtr<FWindow>& Window = Windows.at(Id);
		Window.reset();
		Windows.erase(Id);
	}
}
