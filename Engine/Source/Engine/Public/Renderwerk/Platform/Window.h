#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Platform/WindowEvents.h"

enum class ENGINE_API EWindowStyle : uint8
{
	None = 0,
	Windowed,
	Borderless,
	Fullscreen,
};

enum class ENGINE_API EWindowFlags : uint8
{
	None = 0,
	Resizable = BIT(0),
	Minimizable = BIT(1),
	Maximizable = BIT(2),
	All = Resizable | Minimizable | Maximizable,
};

enum class ENGINE_API EWindowVisibility : uint8
{
	None = 0,
	Visible,
	Hidden,
	Closed,
};

struct ENGINE_API FWindowDesc
{
	uint32 Width = 1280;
	uint32 Height = 720;
	FString Title = TEXT("Renderwerk");
	EWindowStyle Style = EWindowStyle::Windowed;
	TFlags<EWindowFlags> Flags = EWindowFlags::All;
	bool8 bShowOnCreation = true;
};

struct ENGINE_API FWindowState
{
	uint32 Width = 0;
	uint32 Height = 0;
	uint32 ClientWidth = 0;
	uint32 ClientHeight = 0;
	int32 PositionX = 0;
	int32 PositionY = 0;
	FString Title;
	EWindowStyle Style = EWindowStyle::Windowed;
	TFlags<EWindowFlags> Flags = EWindowFlags::All;
	EWindowVisibility Visibility = EWindowVisibility::None;
	bool8 bIsFocused = false;
};

class ENGINE_API FWindow
{
public:
	FWindow(const FGuid& InId, const FWindowDesc& InDescription);
	~FWindow();

	DELETE_COPY_AND_MOVE(FWindow);

public:
	[[nodiscard]] bool8 IsValid() const;

	void Show() const;
	void Hide() const;

public:
	[[nodiscard]] HWND GetHandle() const { return WindowHandle; }
	[[nodiscard]] FGuid GetId() const { return Id; }

	[[nodiscard]] const FWindowState& GetState() const { return State; }
	[[nodiscard]] FWindowEventQueue& GetEventQueue() { return EventQueue; }

private:
	[[nodiscard]] LRESULT WindowProc(HWND InWindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

	int64 OnCloseMessage(HWND InWindowHandle);
	void OnDestroyMessage();
	int64 OnSizeMessage(HWND InWindowHandle, WPARAM WParam, LPARAM LParam);
	int64 OnMoveMessage(LPARAM LParam);
	int64 OnEnterSizeMoveMessage();
	int64 OnExitSizeMoveMessage();
	int64 OnSetFocusMessage();
	int64 OnKillFocusMessage();

private:
	static uint32 GetWindowStyle(EWindowStyle Style, TFlags<EWindowFlags> Flags);
	static uint32 GetWindowExStyle(EWindowStyle Style);

private:
	FGuid Id;
	FWindowState State;

	HWND WindowHandle = nullptr;

	FWindowEventQueue EventQueue;

	uint32 SizeMoveStartWidth = 0;
	uint32 SizeMoveStartHeight = 0;
	int32 SizeMoveStartPositionX = 0;
	int32 SizeMoveStartPositionY = 0;

	friend LRESULT WindowProc(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
};
