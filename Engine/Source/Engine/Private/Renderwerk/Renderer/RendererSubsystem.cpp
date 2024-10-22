#include "pch.h"

#include "Renderwerk/Renderer/RendererSubsystem.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Windowing/Window.h"
#include "Renderwerk/Platform/Windowing/WindowSubsystem.h"
#include "Renderwerk/RHI/RHIBackend.h"
#include "Renderwerk/RHI/Components/Device.h"
#include "Renderwerk/RHI/Components/Swapchain.h"
#include "Renderwerk/RHI/Synchronization/Fence.h"

DEFINE_LOG_CATEGORY(LogRenderer);

FRendererSubsystem::FRendererSubsystem() = default;

FRendererSubsystem::~FRendererSubsystem() = default;

TSharedPtr<FFence> Fence;

void FRendererSubsystem::Initialize()
{
	RW_PROFILING_MARK_FUNCTION();

	OnTickHandle = GetEngine()->GetTickDelegate()->Bind(BIND_MEMBER_ONE(FRendererSubsystem::OnTick));

	WindowSubsystem = GetSubsystem<FWindowSubsystem>();
	Window = WindowSubsystem->GetWindow(WindowSubsystem->GetMainWindowId());
	ASSERTM(Window, "Failed to get main window");

	Backend = MakeLocal<FRHIBackend>();

	FSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Window = Window;
	Swapchain = Backend->GetDevice()->CreateSwapchain(SwapchainDesc);

	Fence = Backend->GetDevice()->CreateFence();

	RW_LOG(LogRenderer, Info, "Renderer subsystem initialized");
}

void FRendererSubsystem::Shutdown()
{
	Backend->GetDevice()->WaitForGraphicsQueueIdle();

	Fence.reset();
	Swapchain.reset();
	Backend.reset();
	GetEngine()->GetTickDelegate()->Unbind(OnTickHandle);
}

void FRendererSubsystem::ProcessWindowEvents(TQueue<FEventPtr>& Events) const
{
	RW_PROFILING_MARK_FUNCTION();

	if (Events.empty())
		return;

	do
	{
		FEventPtr Event = Events.front();
		Events.pop();

		switch (Event->Type)
		{
		case EWindowEventType::ClientAreaResized:
			{
				TSharedPtr<FWindowClientAreaResizedEvent> ResizedEvent = CastEvent<FWindowClientAreaResizedEvent>(Event);
				OnClientAreaResized(ResizedEvent->Width, ResizedEvent->Height);
			}
			break;
		case EWindowEventType::FullscreenStateChanged:
			{
				TSharedPtr<FWindowFullscreenStateChangedEvent> FullscreenEvent = CastEvent<FWindowFullscreenStateChangedEvent>(Event);
				OnFullscreenStateChanged(FullscreenEvent->bState);
			}
			break;
		default:
			break;
		}
	}
	while (!Events.empty());
}

void FRendererSubsystem::OnTick(MAYBE_UNUSED float64 DeltaTime) const
{
	RW_PROFILING_MARK_FUNCTION();

	if (!Window || !Window->IsValid())
		return;

	WindowSubsystem->GetEventQueue().SwapContainers();
	ProcessWindowEvents(WindowSubsystem->GetEventQueue().GetBackContainer());

	Swapchain->Present();
}

void FRendererSubsystem::OnClientAreaResized(const int32 Width, const int32 Height) const
{
	RW_PROFILING_MARK_FUNCTION();

	Backend->GetDevice()->WaitForGraphicsQueueIdle();

	Swapchain->Resize(Width, Height);
	RW_LOG(LogRenderer, Trace, "Resized to {}x{}", Width, Height);
}

void FRendererSubsystem::OnFullscreenStateChanged(const bool8 bState) const
{
	Swapchain->SetWindowedFullscreen(bState);

	RW_LOG(LogRenderer, Trace, "Fullscreen state changed to {}", bState);
}
