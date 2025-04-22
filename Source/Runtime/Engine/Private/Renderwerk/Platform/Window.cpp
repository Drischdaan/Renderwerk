#include "pch.hpp"

#include "Renderwerk/Platform/Window.hpp"

FWindow::FWindow(const WNDCLASSEX& InWindowClass, const FGuid& InGuid, const FWindowDesc& InDescription)
	: Guid(InGuid)
{
	State = {};
	State.WindowWidth = InDescription.Width;
	State.WindowHeight = InDescription.Height;
	State.ClientWidth = InDescription.Width;
	State.ClientHeight = InDescription.Height;
	State.PositionX = InDescription.PositionX;
	State.PositionY = InDescription.PositionY;
	State.Title = InDescription.Title;

	ParentWindow = InDescription.ParentWindow;

	const uint32 ExStyle = GetExWindowStyleByType(InDescription.Type);
	const uint32 Style = GetWindowStyleByType(InDescription.Type);

	if (Style & WS_BORDER)
	{
		RECT WindowRect = {0, 0, static_cast<LONG>(State.ClientWidth), static_cast<LONG>(State.ClientHeight)};
		AdjustWindowRectEx(&WindowRect, Style, false, ExStyle);
		State.WindowWidth = WindowRect.right - WindowRect.left;
		State.WindowHeight = WindowRect.bottom - WindowRect.top;
	}

	HWND ParentWindowHandle = nullptr;
	if (ParentWindow)
	{
		ParentWindowHandle = ParentWindow->WindowHandle;
	}
	WindowHandle = CreateWindowEx(ExStyle,
	                              InWindowClass.lpszClassName,
	                              State.Title.c_str(),
	                              Style,
	                              State.PositionX, State.PositionY,
	                              static_cast<int32>(State.WindowWidth), static_cast<int32>(State.WindowHeight),
	                              ParentWindowHandle,
	                              nullptr,
	                              InWindowClass.hInstance,
	                              this
	);
}

FWindow::~FWindow()
{
	if (WindowHandle)
	{
		DestroyWindow(WindowHandle);
		WindowHandle = nullptr;
	}
}

void FWindow::Show() const
{
	ShowWindowAsync(WindowHandle, SW_SHOW);
}

void FWindow::Hide() const
{
	ShowWindowAsync(WindowHandle, SW_HIDE);
}

void FWindow::Minimize() const
{
	ShowWindowAsync(WindowHandle, SW_MINIMIZE);
}

void FWindow::Maximize() const
{
	ShowWindowAsync(WindowHandle, SW_MAXIMIZE);
}

void FWindow::Focus() const
{
	SetFocus(WindowHandle);
}

void FWindow::Close() const
{
	CloseWindow(WindowHandle);
}

void FWindow::SetPosition(const int32 NewPositionX, const int32 NewPositionY) const
{
	SetWindowPos(WindowHandle, nullptr, NewPositionX, NewPositionY, 0, 0, SWP_NOSIZE);
}

void FWindow::SetWindowSize(const uint32 NewWidth, const uint32 NewHeight) const
{
	SetWindowPos(WindowHandle, nullptr, 0, 0, static_cast<int32>(NewWidth), static_cast<int32>(NewHeight), SWP_NOMOVE);
}

void FWindow::SetClientSize(const uint32 NewWidth, const uint32 NewHeight) const
{
	RECT WindowRect = {0, 0, static_cast<LONG>(NewWidth), static_cast<LONG>(NewHeight)};
	if (AdjustWindowRect(&WindowRect, GetWindowLong(WindowHandle, GWL_STYLE), false))
	{
		const int32 AdjustedWidth = WindowRect.right - WindowRect.left;
		const int32 AdjustedHeight = WindowRect.bottom - WindowRect.top;
		SetWindowPos(WindowHandle, nullptr, 0, 0, AdjustedWidth, AdjustedHeight, SWP_NOMOVE);
	}
}

void FWindow::SetTitle(const FString& NewTitle)
{
	State.Title = NewTitle;
	SetWindowText(WindowHandle, State.Title.c_str());
}

void FWindow::AppendTitle(const FString& NewTitle)
{
	State.Title += NewTitle;
	SetWindowText(WindowHandle, State.Title.c_str());
}

void FWindow::SetParentWindow(const TRef<FWindow>& NewParent)
{
	ParentWindow = NewParent;
	if (ParentWindow != nullptr)
	{
		SetParent(WindowHandle, ParentWindow->WindowHandle);
	}
}

LRESULT FWindow::WindowProcess(const HWND ProcessWindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
{
	switch (Message)
	{
	case WM_CLOSE: return OnCloseMessage(ProcessWindowHandle);
	case WM_DESTROY: return OnDestroyMessage();
	case WM_SIZE: return OnSizeMessage(ProcessWindowHandle, LParam);
	case WM_MOVE: return OnMoveMessage(LParam);
	case WM_ENTERSIZEMOVE: return OnEnterSizeMoveMessage();
	case WM_EXITSIZEMOVE: return OnExitSizeMoveMessage(ProcessWindowHandle);
	default:
		break;
	}
	return DefWindowProc(ProcessWindowHandle, Message, WParam, LParam);
}

int64 FWindow::OnCloseMessage(const HWND ProcessWindowHandle)
{
	State.bIsClosed = true;
	DestroyWindow(ProcessWindowHandle);
	return 0;
}

int64 FWindow::OnDestroyMessage()
{
	State.bIsDestroyed = true;
	WindowHandle = nullptr;
	return 0;
}

int64 FWindow::OnSizeMessage(const HWND ProcessWindowHandle, const LPARAM LParam)
{
	if (bIsInModalLoop)
	{
		return 0;
	}
	State.ClientWidth = LOWORD(LParam);
	State.ClientHeight = HIWORD(LParam);

	RECT WindowRect = {};
	GetWindowRect(ProcessWindowHandle, &WindowRect);
	State.WindowWidth = WindowRect.right - WindowRect.left;
	State.WindowHeight = WindowRect.bottom - WindowRect.top;
	return 0;
}

int64 FWindow::OnMoveMessage(const LPARAM LParam)
{
	if (bIsInModalLoop)
	{
		return 0;
	}
	State.PositionX = LOWORD(LParam);
	State.PositionY = HIWORD(LParam);
	return 0;
}

int64 FWindow::OnEnterSizeMoveMessage()
{
	bIsInModalLoop = true;
	ModalState = {};
	ModalState.Width = State.WindowWidth;
	ModalState.Height = State.WindowHeight;
	ModalState.PositionX = State.PositionX;
	ModalState.PositionY = State.PositionY;
	return 0;
}

int64 FWindow::OnExitSizeMoveMessage(const HWND ProcessWindowHandle)
{
	bIsInModalLoop = false;

	RECT WindowRect = {};
	GetWindowRect(ProcessWindowHandle, &WindowRect);

	FModalState AfterModalState;
	AfterModalState.Width = WindowRect.right - WindowRect.left;
	AfterModalState.Height = WindowRect.bottom - WindowRect.top;
	AfterModalState.PositionX = WindowRect.left;
	AfterModalState.PositionY = WindowRect.top;

	if (AfterModalState.Width != ModalState.Width || AfterModalState.Height != ModalState.Height)
	{
		State.WindowWidth = AfterModalState.Width;
		State.WindowHeight = AfterModalState.Height;

		RECT ClientRect = {};
		GetClientRect(ProcessWindowHandle, &ClientRect);
		State.ClientWidth = ClientRect.right - ClientRect.left;
		State.ClientHeight = ClientRect.bottom - ClientRect.top;
	}

	if (AfterModalState.PositionX != ModalState.PositionX || AfterModalState.PositionY != ModalState.PositionY)
	{
		State.PositionX = AfterModalState.PositionX;
		State.PositionY = AfterModalState.PositionY;
	}

	return 0;
}

uint32 FWindow::GetWindowStyleByType(const EWindowType Type)
{
	uint32 Style = 0;
	switch (Type)
	{
	case EWindowType::Default:
		Style |= WS_OVERLAPPEDWINDOW;
		break;
	case EWindowType::Tool:
		Style |= WS_POPUP;
		break;
	}
	return Style;
}

uint32 FWindow::GetExWindowStyleByType(const EWindowType Type)
{
	uint32 ExStyle = 0;
	switch (Type)
	{
	case EWindowType::Default:
		ExStyle |= WS_EX_APPWINDOW;
		break;
	case EWindowType::Tool:
		ExStyle |= WS_EX_TOOLWINDOW;
		break;
	}
	return ExStyle;
}
