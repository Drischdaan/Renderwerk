#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/CoreMacros.hpp"
#include "Renderwerk/Core/Memory/SmartPointer.hpp"
#include "Renderwerk/Core/Misc/Guid.hpp"

enum class ENGINE_API EWindowType : uint8
{
	Default = 0,
	Tool,
};

struct ENGINE_API FWindowDesc
{
	uint32 Width = 1280;
	uint32 Height = 720;
	int32 PositionX = 250;
	int32 PositionY = 250;
	EWindowType Type = EWindowType::Default;
	FString Title = TEXT("Renderwerk");
	TRef<class FWindow> ParentWindow = nullptr;
};

struct ENGINE_API FWindowState
{
	uint32 WindowWidth = 0;
	uint32 WindowHeight = 0;
	uint32 ClientWidth = 0;
	uint32 ClientHeight = 0;
	int32 PositionX = 0;
	int32 PositionY = 0;
	FString Title;
	bool8 bIsClosed = false;
	bool8 bIsDestroyed = false;
};

class ENGINE_API FWindow
{
private:
	struct FModalState
	{
		uint32 Width = 0;
		uint32 Height = 0;
		int32 PositionX = 0;
		int32 PositionY = 0;
	};

public:
	FWindow(const WNDCLASSEX& InWindowClass, const FGuid& InGuid, const FWindowDesc& InDescription);
	~FWindow();

	DEFAULT_COPY_MOVEABLE(FWindow)

public:
	void Show() const;
	void Hide() const;
	void Minimize() const;
	void Maximize() const;
	void Focus() const;
	void Close() const;

	void SetPosition(int32 NewPositionX, int32 NewPositionY) const;
	void SetWindowSize(uint32 NewWidth, uint32 NewHeight) const;
	void SetClientSize(uint32 NewWidth, uint32 NewHeight) const;

	void SetTitle(const FString& NewTitle);
	void AppendTitle(const FString& NewTitle);

	void SetParentWindow(const TRef<FWindow>& NewParent);

public:
	[[nodiscard]] HWND GetHandle() const { return WindowHandle; }

	[[nodiscard]] FGuid GetGuid() const { return Guid; }

	[[nodiscard]] FWindowState GetState() const { return State; }

private:
	LRESULT WindowProcess(HWND ProcessWindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

	int64 OnCloseMessage(HWND ProcessWindowHandle);
	int64 OnDestroyMessage();
	int64 OnSizeMessage(HWND ProcessWindowHandle, LPARAM LParam);
	int64 OnMoveMessage(LPARAM LParam);
	int64 OnEnterSizeMoveMessage();
	int64 OnExitSizeMoveMessage(HWND ProcessWindowHandle);

private:
	static uint32 GetExWindowStyleByType(EWindowType Type);
	static uint32 GetWindowStyleByType(EWindowType Type);

private:
	FGuid Guid;

	HWND WindowHandle;

	FWindowState State;
	TRef<FWindow> ParentWindow;

	bool8 bIsInModalLoop = false;
	FModalState ModalState = {};

	friend LRESULT WindowProcess(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
};
