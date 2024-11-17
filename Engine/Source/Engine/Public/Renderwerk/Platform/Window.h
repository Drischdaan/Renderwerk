#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Platform/WindowEvents.h"

#include <Windows.h>

enum class ENGINE_API EWindowStyle : uint8
{
	None = 0,
	Default,
	Borderless,
};

struct ENGINE_API FWindowDesc
{
	uint32 Width = 1280;
	uint32 Height = 720;
	EWindowStyle Style = EWindowStyle::Default;
	FString Title = "Renderwerk";
	bool8 bIsVisibleOnCreation = true;
	class FWindow* ParentWindow = nullptr;
};

struct ENGINE_API FWindowState
{
	uint32 Width = 0;
	uint32 Height = 0;
	uint32 ClientAreaWidth = 0;
	uint32 ClientAreaHeight = 0;
	bool8 bIsVisible = false;
	bool8 bIsValid = true;
	int32 PositionX = 0;
	int32 PositionY = 0;
	bool8 bIsFocused = false;
	FString Title;
};

class ENGINE_API FWindow
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
	[[nodiscard]] HWND GetHandle() const { return WindowHandle; }
	[[nodiscard]] FGuid GetId() const { return Id; }

	[[nodiscard]] const FWindowState& GetState() const { return State; }

	[[nodiscard]] FEventQueue& GetEventQueue() { return EventQueue; }

private:
	[[nodiscard]] LRESULT WindowProc(HWND InWindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

	int64 OnCloseMessage(HWND InWindowHandle);
	int64 OnDestroyMessage();
	int64 OnSizeMessage(HWND InWindowHandle, WPARAM WParam, LPARAM LParam);
	int64 OnMoveMessage(LPARAM LParam);
	int64 OnEnterSizeMoveMessage();
	int64 OnExitSizeMoveMessage();
	int64 OnSetFocusMessage();
	int64 OnKillFocusMessage();

private:
	static uint32 GetWindowStyleFromDescription(const FWindowDesc& InDescription);
	static uint32 GetWindowExStyleFromDescription(const FWindowDesc& InDescription);

private:
	FGuid Id;
	FWindowDesc Description;

	FWindowState State;
	HWND WindowHandle;

	FEventQueue EventQueue;

	uint32 SizeMoveStartWidth = 0;
	uint32 SizeMoveStartHeight = 0;
	int32 SizeMoveStartPositionX = 0;
	int32 SizeMoveStartPositionY = 0;

	friend LRESULT WindowProc(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
};
