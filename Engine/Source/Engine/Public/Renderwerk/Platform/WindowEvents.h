#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Threading/BufferedContainer.h"

#include <concepts>

enum class ENGINE_API EWindowEventType : uint8
{
	None = 0,
	Resize,
	ClientResize,
	Move,
};

struct ENGINE_API IWindowEvent
{
	IWindowEvent() = default;
	IWindowEvent(EWindowEventType InType);
	virtual ~IWindowEvent() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(IWindowEvent);

public:
	EWindowEventType Type = EWindowEventType::None;
};

using FWindowEvent = TSharedPtr<IWindowEvent>;
using FWindowEventQueue = TBufferedContainer<TDeque<FWindowEvent>, FWindowEvent>;

template <typename T> requires std::derived_from<T, IWindowEvent>
ENGINE_API TSharedPtr<T> CastEvent(const TSharedPtr<IWindowEvent>& Event)
{
	return std::static_pointer_cast<T>(Event);
}

/**
 * Sorts out duplicate events, that shouldn't be processed
 * @param Events The events to process
 * @return The processed events
 */
[[nodiscard]] ENGINE_API TDeque<FWindowEvent> ProcessWindowEvents(const TDeque<FWindowEvent>& Events);

struct ENGINE_API FWindowResizeEvent : IWindowEvent
{
	FWindowResizeEvent();
	FWindowResizeEvent(uint32 InWidth, uint32 InHeight);
	~FWindowResizeEvent() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FWindowResizeEvent);

public:
	uint32 Width = 0;
	uint32 Height = 0;
};

struct ENGINE_API FWindowClientResizeEvent : IWindowEvent
{
	FWindowClientResizeEvent();
	FWindowClientResizeEvent(uint32 InWidth, uint32 InHeight);
	~FWindowClientResizeEvent() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FWindowClientResizeEvent);

public:
	uint32 Width = 0;
	uint32 Height = 0;
};

struct ENGINE_API FWindowMoveEvent : IWindowEvent
{
	FWindowMoveEvent();
	FWindowMoveEvent(int32 InPositionX, int32 InPositionY);
	~FWindowMoveEvent() override = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FWindowMoveEvent);

public:
	int32 PositionX = 0;
	int32 PositionY = 0;
};
