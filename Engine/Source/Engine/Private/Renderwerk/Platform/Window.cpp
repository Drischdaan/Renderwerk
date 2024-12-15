#include "pch.h"

#include "Renderwerk/Platform/Window.h"

#include "Renderwerk/Platform/WindowManager.h"

FWindow::FWindow(const FGuid& InId, const FWindowDesc& InDescription)
	: Id(InId)
{
	State = {};
	State.Width = InDescription.Width;
	State.Height = InDescription.Height;
	State.Title = InDescription.Title;
	State.Style = InDescription.Style;
	State.Flags = InDescription.Flags;

	const WNDCLASSEX WindowClass = GetEngine()->GetWindowManager()->GetWindowClass();
	WindowHandle = CreateWindowEx(GetWindowExStyle(State.Style),
	                              WindowClass.lpszClassName,
	                              State.Title.c_str(),
	                              GetWindowStyle(State.Style, State.Flags),
	                              CW_USEDEFAULT, CW_USEDEFAULT,
	                              State.Width, State.Height,
	                              nullptr,
	                              nullptr,
	                              WindowClass.hInstance,
	                              this);
	if (InDescription.bShowOnCreation)
		Show();
	ASSERT(WindowHandle, "Failed to create window");
}

FWindow::~FWindow()
{
	if (WindowHandle && IsWindow(WindowHandle))
		DestroyWindow(WindowHandle);
}

bool8 FWindow::IsValid() const
{
	return WindowHandle && IsWindow(WindowHandle);
}

void FWindow::Show() const
{
	ShowWindow(WindowHandle, SW_SHOW);
}

void FWindow::Hide() const
{
	ShowWindow(WindowHandle, SW_HIDE);
}

void FWindow::SignalCloseCondition()
{
	CloseCondition.notify_all();
}

LRESULT FWindow::WindowProc(const HWND InWindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
{
	switch (Message)
	{
	case WM_CLOSE:
		return OnCloseMessage(InWindowHandle);
	case WM_DESTROY:
		OnDestroyMessage();
		break;
	case WM_SIZE:
		return OnSizeMessage(InWindowHandle, WParam, LParam);
	case WM_MOVE:
		return OnMoveMessage(LParam);
	case WM_ENTERSIZEMOVE:
		return OnEnterSizeMoveMessage();
	case WM_EXITSIZEMOVE:
		return OnExitSizeMoveMessage();
	case WM_SETFOCUS:
		return OnSetFocusMessage();
	case WM_KILLFOCUS:
		return OnKillFocusMessage();
	default:
		break;
	}
	return DefWindowProc(InWindowHandle, Message, WParam, LParam);
}

int64 FWindow::OnCloseMessage(const HWND InWindowHandle)
{
	FUniqueLock Lock(CloseMutex);
	CloseCondition.wait(Lock);
	State.Visibility = EWindowVisibility::Closed;
	DestroyWindow(InWindowHandle);
	return 0;
}

void FWindow::OnDestroyMessage()
{
	WindowHandle = nullptr;
}

int64 FWindow::OnSizeMessage(const HWND InWindowHandle, const WPARAM WParam, const LPARAM LParam)
{
	const bool8 bWasHidden = State.Visibility == EWindowVisibility::Hidden;

	State.ClientWidth = LOWORD(LParam);
	State.ClientHeight = HIWORD(LParam);

	RECT WindowRect = {};
	VERIFY(GetWindowRect(InWindowHandle, &WindowRect), "Failed to get window rect");
	State.Width = WindowRect.right - WindowRect.left;
	State.Height = WindowRect.bottom - WindowRect.top;

	if (WParam == SIZE_MAXIMIZED)
	{
		State.Visibility = EWindowVisibility::Visible;
		EventQueue.Emplace(MakeShared<FWindowResizeEvent>(State.Width, State.Height));
		EventQueue.Emplace(MakeShared<FWindowClientResizeEvent>(State.ClientWidth, State.ClientHeight));
	}
	else if (WParam == SIZE_MINIMIZED)
	{
		State.Visibility = EWindowVisibility::Hidden;
		EventQueue.Emplace(MakeShared<FWindowResizeEvent>(State.Width, State.Height));
		EventQueue.Emplace(MakeShared<FWindowClientResizeEvent>(State.ClientWidth, State.ClientHeight));
	}
	else if (WParam == SIZE_RESTORED)
	{
		if (State.ClientWidth == 0 || State.ClientHeight == 0)
			State.Visibility = EWindowVisibility::Hidden;
		else
			State.Visibility = EWindowVisibility::Visible;
		if (bWasHidden)
		{
			EventQueue.Emplace(MakeShared<FWindowResizeEvent>(State.Width, State.Height));
			EventQueue.Emplace(MakeShared<FWindowClientResizeEvent>(State.ClientWidth, State.ClientHeight));
		}
	}
	return 0;
}

int64 FWindow::OnMoveMessage(const LPARAM LParam)
{
	State.PositionX = LOWORD(LParam);
	State.PositionY = HIWORD(LParam);
	return 0;
}

int64 FWindow::OnEnterSizeMoveMessage()
{
	SizeMoveStartWidth = State.Width;
	SizeMoveStartHeight = State.Height;
	SizeMoveStartPositionX = State.PositionX;
	SizeMoveStartPositionY = State.PositionY;
	return 0;
}

int64 FWindow::OnExitSizeMoveMessage()
{
	if (SizeMoveStartWidth != State.Width || SizeMoveStartHeight != State.Height)
	{
		EventQueue.Emplace(MakeShared<FWindowResizeEvent>(State.Width, State.Height));
		EventQueue.Emplace(MakeShared<FWindowClientResizeEvent>(State.ClientWidth, State.ClientHeight));
	}
	if (SizeMoveStartPositionX != State.PositionX || SizeMoveStartPositionY != State.PositionY)
		EventQueue.Emplace(MakeShared<FWindowMoveEvent>(State.PositionX, State.PositionY));
	return 0;
}

int64 FWindow::OnSetFocusMessage()
{
	State.bIsFocused = true;
	return 0;
}

int64 FWindow::OnKillFocusMessage()
{
	State.bIsFocused = false;
	return 0;
}

uint32 FWindow::GetWindowStyle(const EWindowStyle Style, const TFlags<EWindowFlags> Flags)
{
	uint32 WindowStyle = 0;
	switch (Style)
	{
	case EWindowStyle::Windowed:
		WindowStyle |= WS_OVERLAPPEDWINDOW;
		break;
	case EWindowStyle::Borderless:
		WindowStyle |= WS_POPUP;
		break;
	case EWindowStyle::Fullscreen:
		WindowStyle |= WS_POPUP;
		break;
	case EWindowStyle::None:
	default:
		break;
	}
	if (Style == EWindowStyle::Windowed && !(Flags & EWindowFlags::Resizable))
		WindowStyle &= ~WS_THICKFRAME;
	if (Style == EWindowStyle::Windowed && !(Flags & EWindowFlags::Minimizable))
		WindowStyle &= ~WS_MINIMIZEBOX;
	if (Style == EWindowStyle::Windowed && !(Flags & EWindowFlags::Maximizable))
		WindowStyle &= ~WS_MAXIMIZEBOX;
	return WindowStyle;
}

uint32 FWindow::GetWindowExStyle(const EWindowStyle Style)
{
	uint32 WindowExStyle = 0;
	switch (Style)
	{
	case EWindowStyle::Windowed:
		WindowExStyle |= WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
		break;
	case EWindowStyle::Borderless:
		WindowExStyle |= WS_EX_APPWINDOW;
		break;
	case EWindowStyle::Fullscreen:
		WindowExStyle |= WS_EX_APPWINDOW;
		break;
	case EWindowStyle::None:
	default:
		break;
	}
	return WindowExStyle;
}
