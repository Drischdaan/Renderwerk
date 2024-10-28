#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

enum class RENDERWERK_API EWindowEventType : uint8
{
	None = 0,
	Resized,
	ClientAreaResized,
	FocusChanged,
	FullscreenStateChanged,
};

struct RENDERWERK_API IWindowEvent
{
public:
	IWindowEvent() = default;

	IWindowEvent(const EWindowEventType InType)
		: Type(InType)
	{
	}

	virtual ~IWindowEvent() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(IWindowEvent);

public:
	EWindowEventType Type = EWindowEventType::None;
};

template <typename T, typename = std::is_base_of<IWindowEvent, T>>
RENDERWERK_API TSharedPtr<T> CastEvent(const TSharedPtr<IWindowEvent>& Event)
{
	return std::static_pointer_cast<T>(Event);
}

struct FWindowResizedEvent : IWindowEvent
{
public:
	FWindowResizedEvent()
		: IWindowEvent(EWindowEventType::Resized)
	{
	}

	FWindowResizedEvent(const uint32 InWidth, const uint32 InHeight)
		: IWindowEvent(EWindowEventType::Resized), Width(InWidth), Height(InHeight)
	{
	}

	~FWindowResizedEvent() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FWindowResizedEvent);

public:
	uint32 Width = 0;
	uint32 Height = 0;
};

struct FWindowClientAreaResizedEvent : IWindowEvent
{
public:
	FWindowClientAreaResizedEvent()
		: IWindowEvent(EWindowEventType::ClientAreaResized)
	{
	}

	FWindowClientAreaResizedEvent(const uint32 InWidth, const uint32 InHeight)
		: IWindowEvent(EWindowEventType::ClientAreaResized), Width(InWidth), Height(InHeight)
	{
	}

	~FWindowClientAreaResizedEvent() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FWindowClientAreaResizedEvent);

public:
	uint32 Width = 0;
	uint32 Height = 0;
};

struct FWindowFocusChangedEvent : IWindowEvent
{
public:
	FWindowFocusChangedEvent()
		: IWindowEvent(EWindowEventType::FocusChanged)
	{
	}

	FWindowFocusChangedEvent(const bool8 InIsFocused)
		: IWindowEvent(EWindowEventType::FocusChanged), bIsFocused(InIsFocused)
	{
	}

	~FWindowFocusChangedEvent() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FWindowFocusChangedEvent);

public:
	bool8 bIsFocused = false;
};

struct RENDERWERK_API FWindowFullscreenStateChangedEvent : IWindowEvent
{
public:
	FWindowFullscreenStateChangedEvent()
		: IWindowEvent(EWindowEventType::FullscreenStateChanged)
	{
	}

	FWindowFullscreenStateChangedEvent(const bool8 InState)
		: IWindowEvent(EWindowEventType::FullscreenStateChanged), bState(InState)
	{
	}

	~FWindowFullscreenStateChangedEvent() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FWindowFullscreenStateChangedEvent);

public:
	bool8 bState = false;
};
