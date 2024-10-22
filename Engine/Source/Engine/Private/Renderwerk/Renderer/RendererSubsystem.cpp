#include "pch.h"

#include "Renderwerk/Renderer/RendererSubsystem.h"

#include "Renderwerk/Engine/Engine.h"
#include "Renderwerk/Platform/Window.h"
#include "Renderwerk/Platform/WindowSubsystem.h"
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

	TSharedPtr<FWindowSubsystem> WindowSubsystem = GetSubsystem<FWindowSubsystem>();
	Window = WindowSubsystem->GetWindow(WindowSubsystem->GetMainWindowId());
	ASSERTM(Window, "Failed to get main window");

	Backend = MakeLocal<FRHIBackend>();

	FSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Window = Window;
	Swapchain = Backend->GetDevice()->CreateSwapchain(SwapchainDesc);

	Fence = Backend->GetDevice()->CreateFence();

	OnClientAreaResizedHandle = Window->GetClientAreaResizedDelegate()->Bind(BIND_MEMBER_TWO(FRendererSubsystem::OnClientAreaResized));

	RW_LOG(LogRenderer, Info, "Renderer subsystem initialized");
}

void FRendererSubsystem::Shutdown()
{
	Backend->GetDevice()->WaitForGraphicsQueueIdle();
	if (Window)
		Window->GetClientAreaResizedDelegate()->Unbind(OnClientAreaResizedHandle);
	Fence.reset();
	Swapchain.reset();
	Backend.reset();
	GetEngine()->GetTickDelegate()->Unbind(OnTickHandle);
}

static bool8 bState = false;

void FRendererSubsystem::OnTick(MAYBE_UNUSED float64 DeltaTime) const
{
	RW_PROFILING_MARK_FUNCTION();

	if (!Window || !Window->IsValid())
		return;
	Swapchain->Present();

	if (GetAsyncKeyState(VK_ESCAPE) & 1)
	{
		bState = !bState;
		Swapchain->SetWindowedFullscreen(bState);
	}
}

void FRendererSubsystem::OnClientAreaResized(const int32 Width, const int32 Height) const
{
	RW_PROFILING_MARK_FUNCTION();

	Backend->GetDevice()->WaitForGraphicsQueueIdle();

	Swapchain->Resize(Width, Height);
}
