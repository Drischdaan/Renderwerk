#include "pch.h"

#include "Renderwerk/Platform/WindowEvents.h"

IWindowEvent::IWindowEvent(const EWindowEventType InType)
	: Type(InType)
{
}

TDeque<FWindowEvent> ProcessWindowEvents(const TDeque<FWindowEvent>& Events)
{
	// This function is executed every frame, so it's important to keep it as fast as possible.
	// It's a pretty allocation heavy operation. Could be optimized by using preallocated
	// memory blocks and custom allocator for these containers. Should be optimized in the future.
	// TODO: Optimize allocations in this function
	PROFILE_FUNCTION();
	TVector<FWindowEvent> WindowEvents(Events.begin(), Events.end());
	std::ranges::reverse(WindowEvents);

	TDeque<FWindowEvent> ProcessedEvents;
	TSet<EWindowEventType> ProcessedTypes;
	for (const FWindowEvent& WindowEvent : WindowEvents)
	{
		if (!ProcessedTypes.contains(WindowEvent->Type))
		{
			ProcessedEvents.push_back(WindowEvent);
			ProcessedTypes.insert(WindowEvent->Type);
		}
	}
	return ProcessedEvents;
}

FWindowResizeEvent::FWindowResizeEvent()
	: IWindowEvent(EWindowEventType::Resize)
{
}

FWindowResizeEvent::FWindowResizeEvent(const uint32 InWidth, const uint32 InHeight)
	: IWindowEvent(EWindowEventType::Resize), Width(InWidth), Height(InHeight)
{
}

FWindowClientResizeEvent::FWindowClientResizeEvent()
	: IWindowEvent(EWindowEventType::ClientResize)
{
}

FWindowClientResizeEvent::FWindowClientResizeEvent(const uint32 InWidth, const uint32 InHeight)
	: IWindowEvent(EWindowEventType::ClientResize), Width(InWidth), Height(InHeight)
{
}

FWindowMoveEvent::FWindowMoveEvent()
	: IWindowEvent(EWindowEventType::Move)
{
}

FWindowMoveEvent::FWindowMoveEvent(const int32 InPositionX, const int32 InPositionY)
	: IWindowEvent(EWindowEventType::Move), PositionX(InPositionX), PositionY(InPositionY)
{
}
