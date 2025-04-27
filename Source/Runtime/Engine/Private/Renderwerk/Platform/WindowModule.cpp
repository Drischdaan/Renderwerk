#include "pch.hpp"

#include "Renderwerk/Platform/WindowModule.hpp"

#include "imgui.h"

#include "Renderwerk/Engine/Engine.hpp"
#include "Renderwerk/Platform/Window.hpp"
#include "Renderwerk/Profiler/Profiler.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WindowProcess(const HWND WindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
{
	if (ImGui_ImplWin32_WndProcHandler(WindowHandle, Message, WParam, LParam))
	{
		return true;
	}

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
	{
		return Window->WindowProcess(WindowHandle, Message, WParam, LParam);
	}
	return DefWindowProc(WindowHandle, Message, WParam, LParam);
}

FWindowModule::FWindowModule()
	: IEngineModule(EEngineThreadAffinity::Main)
{
}

TRef<FWindow> FWindowModule::NewWindow(const FWindowDesc& Description)
{
	FGuid Guid = FGuid::Generate();
	TRef<FWindow> Window = NewRef<FWindow>(WindowClass, Guid, Description);
	Windows.push_back(Window);
	RW_LOG(Info, "Created window '{}' (Title: '{}')", Window->GetGuid(), Description.Title);
	OnWindowAdded.Broadcast(Window);
	return Window;
}

void FWindowModule::DestroyWindow(const FGuid& Guid)
{
	const auto WindowIterator = std::ranges::find_if(Windows.begin(), Windows.end(),
	                                                 [Guid](const TRef<FWindow>& OtherWindow)
	                                                 {
		                                                 return OtherWindow->GetGuid() == Guid;
	                                                 });
	if (WindowIterator != Windows.end())
	{
		RW_LOG(Info, "Destroying window '{}' (Title: '{}')", Guid, (*WindowIterator)->GetState().Title);
		OnWindowRemoved.Broadcast(*WindowIterator);
		Windows.erase(WindowIterator);
	}
}

void FWindowModule::DestroyWindow(TRef<FWindow>&& Window)
{
	DestroyWindow(std::move(Window)->GetGuid());
}

TRef<FWindow> FWindowModule::GetWindow(const FGuid& Guid)
{
	const auto WindowIterator = std::ranges::find_if(Windows.begin(), Windows.end(), [Guid](const TRef<FWindow>& OtherWindow)
	{
		return OtherWindow->GetGuid() == Guid;
	});
	if (WindowIterator != Windows.end())
	{
		return *WindowIterator;
	}
	return nullptr;
}

void FWindowModule::Initialize()
{
	WindowClass = {};
	WindowClass.cbSize = sizeof(WindowClass);
	WindowClass.lpszClassName = TEXT("RenderwerkDefaultClass");
	WindowClass.hInstance = GetModuleHandle(nullptr);
	WindowClass.lpfnWndProc = WindowProcess;
	RegisterClassEx(&WindowClass);

	TickDelegateHandle = GetEngine()->GetMainThreadTickDelegate().BindRaw(this, &FWindowModule::OnTick);
}

void FWindowModule::Shutdown()
{
	Windows.clear();
	GetEngine()->GetMainThreadTickDelegate().Unbind(TickDelegateHandle);
	UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
}

void FWindowModule::OnTick()
{
	PROFILE_FUNCTION();
	{
		PROFILE_SCOPE("MessageLoop");

		MSG Message = {};
		while (PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
}
