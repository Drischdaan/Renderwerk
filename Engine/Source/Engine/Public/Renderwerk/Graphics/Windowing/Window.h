#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Graphics/Windowing/WindowEvents.h"

class FWindow;

enum class RENDERWERK_API EWindowVisibility : uint8
{
	None = 0,
	Visible,
	Hidden,
	Closed,
	Destroyed,
};

enum class RENDERWERK_API EWindowStyle : uint8
{
	None = 0,
	Default,
	Borderless,
};

struct RENDERWERK_API FWindowDesc
{
	uint32 Width = 1280;
	uint32 Height = 720;
	EWindowStyle Style = EWindowStyle::Default;
	FString Title = TEXT("Renderwerk");
	bool8 bIsVisibleOnCreation = true;
	TSharedPtr<FWindow> ParentWindow = nullptr;
};

struct RENDERWERK_API FWindowState
{
	uint32 Width = 0;
	uint32 Height = 0;
	uint32 ClientAreaWidth = 0;
	uint32 ClientAreaHeight = 0;
	EWindowVisibility Visibility = EWindowVisibility::None;
	int32 PositionX = 0;
	int32 PositionY = 0;
	bool8 bIsFocused = false;
	FString Title = TEXT("");
};

class RENDERWERK_API FWindow
{
public:
	FWindow(const FGuid& InId, const FWindowDesc& InDescription);
	~FWindow();

	DELETE_COPY_AND_MOVE(FWindow);

public:
	void Show() const;
	void Maximize() const;
	void Hide() const;
	void Minimize() const;
	void Restore() const;
	void Close() const;

	void SetPosition(int32 PositionX, int32 PositionY) const;
	void SetSize(int32 Width, int32 Height) const;
	void SetClientAreaSize(int32 Width, int32 Height) const;

	void SetTitle(const FString& Title);
	void AppendTitle(const FString& Title);

public:
	NODISCARD HWND GetHandle() const { return WindowHandle; }
	NODISCARD FGuid GetId() const { return Id; }

	NODISCARD const FWindowState& GetState() const { return State; }

	NODISCARD FEventQueue& GetEventQueue() { return EventQueue; }

public:
	NODISCARD LRESULT WindowProc(HWND InWindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

	uint64 OnCloseMessage(HWND InWindowHandle);
	uint64 OnDestroyMessage();
	uint64 OnSizeMessage(HWND InWindowHandle, WPARAM WParam, LPARAM LParam);
	uint64 OnMoveMessage(LPARAM LParam);
	uint64 OnEnterSizeMoveMessage();
	uint64 OnExitSizeMoveMessage();
	uint64 OnSetFocusMessage();
	uint64 OnKillFocusMessage();

private:
	static uint32 GetWindowStyleFromDescription(const FWindowDesc& InDescription);
	static uint32 GetWindowExStyleFromDescription(const FWindowDesc& InDescription);

private:
	FGuid Id;
	FWindowDesc Description;

	FWindowState State;
	WNDCLASSEX WindowClass;
	HWND WindowHandle;

	FEventQueue EventQueue;

	uint32 SizeMoveStartWidth = 0;
	uint32 SizeMoveStartHeight = 0;
	int32 SizeMoveStartPositionX = 0;
	int32 SizeMoveStartPositionY = 0;
};
