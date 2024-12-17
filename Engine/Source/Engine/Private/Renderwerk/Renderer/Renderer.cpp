#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include "Renderwerk/Platform/Window.h"

DEFINE_LOG_CHANNEL(LogRenderer);

FRenderer::FRenderer() = default;

FRenderer::~FRenderer() = default;

void FRenderer::Initialize(const FRendererDesc& InDescription)
{
	Description = InDescription;
	GraphicsApi = MakeUnique<FGraphicsApi>();
	GraphicsApi->Initialize();

	Surface = GraphicsApi->CreateSurface(Description.Window);

	const TSharedPtr<FGraphicsAdapter> Adapter = GraphicsApi->ChooseCompatibleAdapter(Surface);
	RW_LOG(LogGraphics, Info, "Chosen adapter: {}", Adapter->GetName());
	RW_LOG(LogGraphics, Info, "\t- Type: {}", GetEnumValueName(Adapter->GetType()));
	RW_LOG(LogGraphics, Info, "\t- Vendor: {}", GetVendorString(Adapter->GetVendor()));
	RW_LOG(LogGraphics, Info, "\t- API version: {}.{}.{}", VK_API_VERSION_MAJOR(Adapter->GetApiVersion()), VK_API_VERSION_MINOR(Adapter->GetApiVersion()),
	       VK_API_VERSION_PATCH(Adapter->GetApiVersion()));
	RW_LOG(LogGraphics, Info, "\t- Driver version: {}", Adapter->GetDriverVersionString());
	GraphicsDevice = GraphicsApi->CreateDevice(Adapter);

	FGraphicsSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Surface = Surface;
	GraphicsSwapchain = GraphicsApi->CreateSwapchain(GraphicsDevice);
	GraphicsSwapchain->Initialize(SwapchainDesc);

	Frames.resize(3);
	for (FGraphicsFrame& Frame : Frames)
	{
		VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
		CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		CommandPoolCreateInfo.pNext = nullptr;
		CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		CommandPoolCreateInfo.queueFamilyIndex = GraphicsDevice->GetAdapter()->GetQueueMetadata(EGraphicsQueueType::Graphics).FamilyIndex;
		FVulkanResult Result = vkCreateCommandPool(GraphicsDevice->GetHandle(), &CommandPoolCreateInfo, GraphicsApi->GetGraphicsContext()->GetAllocator(),
		                                           &Frame.CommandPool);
		VERIFY(Result == VK_SUCCESS, "Failed to create command pool");

		Frame.CommandBuffer = GraphicsApi->AllocateCommandBuffer(GraphicsDevice, Frame.CommandPool);

		VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
		SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		SemaphoreCreateInfo.pNext = nullptr;
		SemaphoreCreateInfo.flags = 0;

		Result = vkCreateSemaphore(GraphicsDevice->GetHandle(), &SemaphoreCreateInfo, GraphicsApi->GetGraphicsContext()->GetAllocator(), &Frame.ImageAvailableSemaphore);
		VERIFY(Result == VK_SUCCESS, "Failed to create image available semaphore");

		Result = vkCreateSemaphore(GraphicsDevice->GetHandle(), &SemaphoreCreateInfo, GraphicsApi->GetGraphicsContext()->GetAllocator(), &Frame.RenderFinishedSemaphore);
		VERIFY(Result == VK_SUCCESS, "Failed to create render finished semaphore");

		VkFenceCreateInfo FenceCreateInfo = {};
		FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		FenceCreateInfo.pNext = nullptr;
		FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		Result = vkCreateFence(GraphicsDevice->GetHandle(), &FenceCreateInfo, GraphicsApi->GetGraphicsContext()->GetAllocator(), &Frame.InFlightFence);
		VERIFY(Result == VK_SUCCESS, "Failed to create in-flight fence");
	}
}

void FRenderer::Destroy()
{
	GraphicsDevice->WaitForIdle();

	for (FGraphicsFrame& Frame : Frames)
	{
		vkDestroyFence(GraphicsDevice->GetHandle(), Frame.InFlightFence, GraphicsApi->GetGraphicsContext()->GetAllocator());
		vkDestroySemaphore(GraphicsDevice->GetHandle(), Frame.RenderFinishedSemaphore, GraphicsApi->GetGraphicsContext()->GetAllocator());
		vkDestroySemaphore(GraphicsDevice->GetHandle(), Frame.ImageAvailableSemaphore, GraphicsApi->GetGraphicsContext()->GetAllocator());
		GraphicsApi->DeallocateCommandBuffer(GraphicsDevice, Frame.CommandPool, Frame.CommandBuffer);
		Frame.CommandBuffer.reset();
		vkDestroyCommandPool(GraphicsDevice->GetHandle(), Frame.CommandPool, GraphicsApi->GetGraphicsContext()->GetAllocator());
	}
	GraphicsSwapchain->Destroy();
	GraphicsDevice->Destroy();
	GraphicsApi->DestroySurface(Surface);
	GraphicsApi->Destroy();
}

void FRenderer::Resize() const
{
	PROFILE_FUNCTION();
	GraphicsDevice->WaitForIdle();

	GraphicsSwapchain->Resize();
}

void FRenderer::BeginFrame()
{
	PROFILE_FUNCTION();
	FGraphicsFrame& Frame = Frames.at(CurrentFrameId);

	{
		PROFILE_SCOPE("vkWaitForFences");
		FVulkanResult Result = vkWaitForFences(GraphicsDevice->GetHandle(), 1, &Frame.InFlightFence, VK_TRUE, UINT64_MAX);
		VERIFY(Result == VK_SUCCESS, "Failed to wait for in-flight fence");
		Result = vkResetFences(GraphicsDevice->GetHandle(), 1, &Frame.InFlightFence);
		VERIFY(Result == VK_SUCCESS, "Failed to reset in-flight fence");
	}

	Frame.DeletionQueue.Process();

	if (!GraphicsSwapchain->AcquireNextImageIndex(Frame.ImageAvailableSemaphore))
	{
		if (Description.Window->IsValid())
			Resize();
	}
	Frame.ImageIndex = GraphicsSwapchain->GetCurrentImageIndex();

	const TSharedPtr<FGraphicsCommandBuffer> CommandBuffer = Frame.CommandBuffer;
	const FGraphicsBackBuffer BackBuffer = GraphicsSwapchain->GetBackBuffer(Frame.ImageIndex);

	CommandBuffer->Reset();
	CommandBuffer->Begin();

	CommandBuffer->TransitionImageLayout(BackBuffer.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	CommandBuffer->BeginDebugLabel("ClearBackBuffer");
	CommandBuffer->ClearImage(BackBuffer.Image, VK_IMAGE_LAYOUT_GENERAL, {0.1f, 0.1f, 0.1f, 1.0f});
	CommandBuffer->EndDebugLabel();
}

void FRenderer::EndFrame()
{
	PROFILE_FUNCTION();
	const FGraphicsFrame& Frame = Frames.at(CurrentFrameId);
	const TSharedPtr<FGraphicsCommandBuffer> CommandBuffer = Frame.CommandBuffer;
	const FGraphicsBackBuffer BackBuffer = GraphicsSwapchain->GetBackBuffer(Frame.ImageIndex);

	CommandBuffer->TransitionImageLayout(BackBuffer.Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	CommandBuffer->End();

	GraphicsApi->SubmitQueue(GraphicsDevice->GetGraphicsQueue(), Frame);

	if (!GraphicsSwapchain->Present(Frame.RenderFinishedSemaphore))
	{
		if (Description.Window->IsValid())
			Resize();
		return;
	}
	CurrentFrameId = (CurrentFrameId + 1) % Frames.size();
}
