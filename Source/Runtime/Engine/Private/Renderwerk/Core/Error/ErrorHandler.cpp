#include "pch.hpp"

#include "Renderwerk/Core/Error/ErrorHandler.hpp"

#include "Renderwerk/Core/CoreMacros.hpp"

class FErrorDialog
{
public:
	FErrorDialog(FString InTitle, FString InDescription, FString InDetails)
		: Title(std::move(InTitle)), Description(std::move(InDescription)), Details(std::move(InDetails))
	{
		Icon = LoadIcon(nullptr, IDI_ERROR);

		WindowClass = {};
		WindowClass.cbSize = sizeof(WNDCLASSEX);
		WindowClass.style = CS_HREDRAW | CS_VREDRAW;
		WindowClass.lpfnWndProc = DialogProc;
		WindowClass.hInstance = GetModuleHandle(nullptr);
		WindowClass.hIcon = Icon;
		WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		WindowClass.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_BTNFACE + 1));
		WindowClass.lpszClassName = TEXT("ErrorDialogClass");
		RegisterClassExW(&WindowClass);

		WindowHandle = CreateWindowEx(
			WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
			WindowClass.lpszClassName,
			Title.c_str(),
			WS_POPUP | WS_CAPTION | WS_SYSMENU,
			CW_USEDEFAULT, CW_USEDEFAULT,
			465, 305,
			nullptr,
			nullptr,
			GetModuleHandle(nullptr),
			this);

		RECT ScreenRect = {};
		ScreenRect.left = 0;
		ScreenRect.top = 0;
		ScreenRect.right = GetSystemMetrics(SM_CXSCREEN);
		ScreenRect.bottom = GetSystemMetrics(SM_CYSCREEN);

		RECT WindowRect;
		GetWindowRect(WindowHandle, &WindowRect);
		int32 PositionX = (ScreenRect.left + ScreenRect.right) / 2 - (WindowRect.right - WindowRect.left) / 2;
		int32 PositionY = (ScreenRect.top + ScreenRect.bottom) / 2 - (WindowRect.bottom - WindowRect.top) / 2;
		SetWindowPos(WindowHandle, nullptr, PositionX, PositionY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	~FErrorDialog()
	{
		DestroyWindow(WindowHandle);
		UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
	}

	DEFAULT_COPY_MOVEABLE(FErrorDialog)

public:
	void Show()
	{
		ShowWindow(WindowHandle, SW_SHOW);
		UpdateWindow(WindowHandle);

		while (bShouldRun)
		{
			MSG Message = {};
			while (PeekMessage(&Message, WindowHandle, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
				if (Message.message == WM_DESTROY)
				{
					bShouldRun = false;
					break;
				}
			}
		}
	}

private:
	static constexpr int IDC_TEXTBOX = 101;
	static constexpr int IDC_OK_BUTTON = 102;

	static LRESULT CALLBACK DialogProc(const HWND WindowHandle, const UINT Message, const WPARAM WParam, const LPARAM LParam)
	{
		FErrorDialog* Dialog;
		if (Message == WM_CREATE)
		{
			const CREATESTRUCTW* CreateStruct = reinterpret_cast<CREATESTRUCTW*>(LParam);
			Dialog = static_cast<FErrorDialog*>(CreateStruct->lpCreateParams);
			SetWindowLongPtr(WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(Dialog));
		}
		else
		{
			Dialog = reinterpret_cast<FErrorDialog*>(GetWindowLongPtr(WindowHandle, GWLP_USERDATA));
		}
		switch (Message)
		{
		case WM_CREATE:
			{
				HFONT hFont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
				const HWND hIcon = CreateWindowExW(
					0, L"STATIC", nullptr, WS_CHILD | WS_VISIBLE | SS_ICON,
					20, 20, 32, 32, WindowHandle, nullptr, GetModuleHandle(nullptr), nullptr);
				SendMessage(hIcon, STM_SETICON, reinterpret_cast<WPARAM>(Dialog->Icon), 0);
				const HWND hMessage = CreateWindowExW(
					0, L"STATIC", Dialog->Description.c_str(),
					WS_CHILD | WS_VISIBLE | SS_LEFT,
					70, 20, 360, 40, WindowHandle, nullptr, GetModuleHandle(nullptr), nullptr);
				SendMessage(hMessage, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

				const HWND hTextBox = CreateWindowExW(
					WS_EX_CLIENTEDGE, L"EDIT", Dialog->Details.c_str(),
					WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
					20, 70, 410, 150, WindowHandle, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_TEXTBOX)), GetModuleHandle(nullptr), nullptr);
				SendMessage(hTextBox, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

				const HWND hButton = CreateWindowExW(
					0, L"BUTTON", L"OK",
					WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
					350, 230, 80, 25, WindowHandle, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_OK_BUTTON)), GetModuleHandle(nullptr), nullptr);
				SendMessage(hButton, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
				SetFocus(hButton);
				return 0;
			}
		case WM_COMMAND:
			if (LOWORD(WParam) == IDC_OK_BUTTON || LOWORD(WParam) == IDCANCEL)
			{
				Dialog->bShouldRun = false;
				DestroyWindow(WindowHandle);
				return 0;
			}
			break;
		case WM_CLOSE:
			Dialog->bShouldRun = false;
			DestroyWindow(WindowHandle);
			return 0;
		default:
			break;
		}
		return DefWindowProc(WindowHandle, Message, WParam, LParam);
	}

private:
	FString Title;
	FString Description;
	FString Details;

	HICON Icon;
	WNDCLASSEX WindowClass;
	HWND WindowHandle;

	bool8 bShouldRun = true;
};

int32 FErrorHandler::ExitCode = EXIT_SUCCESS;

void FErrorHandler::Report(const FErrorDetails& Details)
{
	ULONG_PTR Arguments[] = {reinterpret_cast<ULONG_PTR>(&Details)};
	RaiseException(ERROR_ID_REPORT, 0, _countof(Arguments), Arguments);
}

long FErrorHandler::Handle(const EXCEPTION_POINTERS* Pointers)
{
	ExitCode = EXIT_FAILURE;
	if (IsDebuggerPresent())
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
	const HRESULT ExceptionCode = static_cast<HRESULT>(Pointers->ExceptionRecord->ExceptionCode);
	if (ExceptionCode == ERROR_ID_REPORT)
	{
		const FErrorDetails* Details = reinterpret_cast<const FErrorDetails*>(Pointers->ExceptionRecord->ExceptionInformation[0]);

		FString Description = TEXT("There was an unhandled issue");
		if (!Details->Description.empty())
		{
			Description = Details->Description;
		}

		FString DialogDetails = TEXT("Context:");
		DialogDetails += TEXT("\r\n - File: ");
		DialogDetails += Details->Context.GetFormattedFile();
		DialogDetails += TEXT("\r\n - Line: ");
		DialogDetails += std::to_wstring(Details->Context.Line);
		DialogDetails += TEXT("\r\n - Function: ");
		DialogDetails += Details->Context.Function;

		FErrorDialog Dialog(TEXT("Renderwerk | Error Handler"), Description, DialogDetails);
		Dialog.Show();
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

int32 FErrorHandler::GetExitCode()
{
	return ExitCode;
}
