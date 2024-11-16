#pragma once

#include "Renderwerk/Core/CoreMinimal.h"
#include "Renderwerk/Platform/Threading/BufferedContainer.h"

enum class ENGINE_API EWindowEventType : uint8
{
	None = 0,
	Resized,
	ClientAreaResized,
	PositionChanged,
	FocusChanged,
};

struct ENGINE_API IWindowEvent
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
ENGINE_API TSharedPtr<T> CastEvent(const TSharedPtr<IWindowEvent>& Event)
{
	return std::static_pointer_cast<T>(Event);
}

struct ENGINE_API FWindowResizedEvent : IWindowEvent
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

struct ENGINE_API FWindowClientAreaResizedEvent : IWindowEvent
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

struct ENGINE_API FWindowMovedEvent : IWindowEvent
{
public:
	FWindowMovedEvent()
		: IWindowEvent(EWindowEventType::PositionChanged)
	{
	}

	FWindowMovedEvent(const int32 InPositionX, const int32 InPositionY)
		: IWindowEvent(EWindowEventType::PositionChanged), PositionX(InPositionX), PositionY(InPositionY)
	{
	}

	~FWindowMovedEvent() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FWindowMovedEvent);

public:
	int32 PositionX = 0;
	int32 PositionY = 0;
};

struct ENGINE_API FWindowFocusChangedEvent : IWindowEvent
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

using FEventPtr = TSharedPtr<IWindowEvent>;
using FEventQueue = TBufferedContainer<TQueue<FEventPtr>, FEventPtr>;
