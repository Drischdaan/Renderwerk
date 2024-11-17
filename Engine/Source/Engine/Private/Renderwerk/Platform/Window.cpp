#include "pch.h"

#include "Renderwerk/Platform/Window.h"

#include "Renderwerk/Platform/WindowManager.h"

FWindow::FWindow(const FGuid& InId, const FWindowDesc& InDescription)
	: Id(InId), Description(InDescription)
{
	const uint32 Style = GetWindowStyleFromDescription(Description);
	const uint32 ExStyle = GetWindowExStyleFromDescription(Description);

	State.Width = Description.Width;
	State.Height = Description.Height;
	State.ClientAreaWidth = Description.Width;
	State.ClientAreaHeight = Description.Height;
	State.bIsVisible = Description.bIsVisibleOnCreation;
	State.PositionX = 0;
	State.PositionY = 0;
	State.bIsFocused = false;
	State.Title = Description.Title;

	HWND ParentHandle = nullptr;
	if (Description.ParentWindow)
		ParentHandle = Description.ParentWindow->GetHandle();
	WindowHandle = CreateWindowExA(ExStyle,
	                               FWindowManager::GetWindowClass().lpszClassName,
	                               Description.Title.c_str(),
	                               Style,
	                               CW_USEDEFAULT, CW_USEDEFAULT,
	                               Description.Width, Description.Height,
	                               ParentHandle,
	                               nullptr,
	                               FWindowManager::GetWindowClass().hInstance,
	                               this);
	ASSERT_MSG(WindowHandle, "Failed to create window");
}

FWindow::~FWindow()
{
	if (WindowHandle && IsWindow(WindowHandle))
		DEBUG_ASSERT_MSG(DestroyWindow(WindowHandle), "Failed to destroy window");
}

void FWindow::Show() const
{
	DEBUG_ASSERT_MSG(ShowWindowAsync(WindowHandle, SW_SHOW), "Failed to show window");
}

void FWindow::Maximize() const
{
	DEBUG_ASSERT_MSG(ShowWindowAsync(WindowHandle, SW_MAXIMIZE), "Failed to maximize window");
}

void FWindow::Hide() const
{
	DEBUG_ASSERT_MSG(ShowWindowAsync(WindowHandle, SW_HIDE), "Failed to hide window");
}

void FWindow::Minimize() const
{
	DEBUG_ASSERT_MSG(ShowWindowAsync(WindowHandle, SW_MINIMIZE), "Failed to minimize window");
}

void FWindow::Restore() const
{
	DEBUG_ASSERT_MSG(ShowWindowAsync(WindowHandle, SW_RESTORE), "Failed to restore window");
}

void FWindow::Close() const
{
	DEBUG_ASSERT_MSG(CloseWindow(WindowHandle), "Failed to close window");
}

void FWindow::SetPosition(const int32 PositionX, const int32 PositionY) const
{
	DEBUG_ASSERT_MSG(SetWindowPos(WindowHandle, nullptr, PositionX, PositionY, 0, 0, SWP_NOSIZE | SWP_NOZORDER), "Failed to set window position");
}

void FWindow::SetSize(const int32 Width, const int32 Height) const
{
	DEBUG_ASSERT_MSG(SetWindowPos(WindowHandle, nullptr, 0, 0, Width, Height, SWP_NOMOVE | SWP_NOZORDER), "Failed to set window size");
}

void FWindow::SetClientAreaSize(const int32 Width, const int32 Height) const
{
	RECT NewSizeRect = {0, 0, Width, Height};
	DEBUG_ASSERT_MSG(AdjustWindowRect(&NewSizeRect, GetWindowLong(WindowHandle, GWL_STYLE), FALSE), "Failed to adjust window rect");
	const uint32 NewWidth = NewSizeRect.right - NewSizeRect.left;
	const uint32 NewHeight = NewSizeRect.bottom - NewSizeRect.top;
	SetSize(NewWidth, NewHeight);
}

void FWindow::SetTitle(const FString& Title)
{
	State.Title = Title;
	DEBUG_ASSERT_MSG(SetWindowTextA(WindowHandle, Title.c_str()), "Failed to set window title");
}

void FWindow::AppendTitle(const FString& Title)
{
	SetTitle(State.Title + Title);
}

LRESULT FWindow::WindowProc(const HWND InWindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
{
	switch (Message)
	{
	case WM_CLOSE:
		return OnCloseMessage(InWindowHandle);
	case WM_DESTROY:
		return OnDestroyMessage();
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
	State.bIsValid = false;
	DestroyWindow(InWindowHandle);
	return 0;
}

int64 FWindow::OnDestroyMessage()
{
	State.bIsValid = false;
	WindowHandle = nullptr;
	return 0;
}

int64 FWindow::OnSizeMessage(const HWND InWindowHandle, const WPARAM WParam, const LPARAM LParam)
{
	const bool8 bWasHidden = !State.bIsVisible;

	State.ClientAreaWidth = LOWORD(LParam);
	State.ClientAreaHeight = HIWORD(LParam);

	RECT WindowRect = {};
	DEBUG_ASSERT_MSG(GetWindowRect(InWindowHandle, &WindowRect), "Failed to get window rect");

	State.Width = WindowRect.right - WindowRect.left;
	State.Height = WindowRect.bottom - WindowRect.top;

	if (WParam == SIZE_MAXIMIZED)
	{
		State.bIsVisible = true;
		EventQueue.Emplace(MakeShared<FWindowResizedEvent>(State.Width, State.Height));
		EventQueue.Emplace(MakeShared<FWindowClientAreaResizedEvent>(State.ClientAreaWidth, State.ClientAreaHeight));
	}
	else if (WParam == SIZE_MINIMIZED)
	{
		State.bIsVisible = false;
		EventQueue.Emplace(MakeShared<FWindowResizedEvent>(State.Width, State.Height));
		EventQueue.Emplace(MakeShared<FWindowClientAreaResizedEvent>(State.ClientAreaWidth, State.ClientAreaHeight));
	}
	else if (WParam == SIZE_RESTORED)
	{
		if (State.ClientAreaWidth == 0 || State.ClientAreaHeight == 0)
			State.bIsVisible = false;
		else
			State.bIsVisible = true;

		if (bWasHidden)
		{
			EventQueue.Emplace(MakeShared<FWindowResizedEvent>(State.Width, State.Height));
			EventQueue.Emplace(MakeShared<FWindowClientAreaResizedEvent>(State.ClientAreaWidth, State.ClientAreaHeight));
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
		EventQueue.Emplace(MakeShared<FWindowResizedEvent>(State.Width, State.Height));
		EventQueue.Emplace(MakeShared<FWindowClientAreaResizedEvent>(State.ClientAreaWidth, State.ClientAreaHeight));
	}

	if (SizeMoveStartPositionX != State.PositionX || SizeMoveStartPositionY != State.PositionY)
		EventQueue.Emplace(MakeShared<FWindowMovedEvent>(State.PositionX, State.PositionY));
	return 0;
}

int64 FWindow::OnSetFocusMessage()
{
	State.bIsFocused = true;
	EventQueue.Emplace(MakeShared<FWindowFocusChangedEvent>(State.bIsFocused));
	return 0;
}

int64 FWindow::OnKillFocusMessage()
{
	State.bIsFocused = false;
	EventQueue.Emplace(MakeShared<FWindowFocusChangedEvent>(State.bIsFocused));
	return 0;
}

uint32 FWindow::GetWindowStyleFromDescription(const FWindowDesc& InDescription)
{
	uint32 Style = 0;
	switch (InDescription.Style)
	{
	case EWindowStyle::Default:
		Style |= WS_OVERLAPPEDWINDOW;
		break;
	case EWindowStyle::Borderless:
		Style |= WS_POPUP;
		break;
	case EWindowStyle::None:
	default:
		break;
	}
	if (InDescription.bIsVisibleOnCreation)
		Style |= WS_VISIBLE;
	if (InDescription.ParentWindow != nullptr)
		Style |= WS_CHILD;
	return Style;
}

uint32 FWindow::GetWindowExStyleFromDescription(const FWindowDesc& InDescription)
{
	uint32 ExStyle = 0;
	switch (InDescription.Style)
	{
	case EWindowStyle::Default:
		ExStyle |= WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
		break;
	case EWindowStyle::Borderless:
		ExStyle |= WS_EX_APPWINDOW;
		break;
	case EWindowStyle::None:
	default:
		break;
	}
	return ExStyle;
}
