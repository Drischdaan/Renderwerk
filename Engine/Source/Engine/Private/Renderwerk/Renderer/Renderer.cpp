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

		VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
		CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		CommandBufferAllocateInfo.pNext = nullptr;
		CommandBufferAllocateInfo.commandPool = Frame.CommandPool;
		CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		CommandBufferAllocateInfo.commandBufferCount = 1;
		Result = vkAllocateCommandBuffers(GraphicsDevice->GetHandle(), &CommandBufferAllocateInfo, &Frame.CommandBuffer);

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

void FRenderer::Destroy() const
{
	GraphicsDevice->WaitForIdle();

	for (const FGraphicsFrame& Frame : Frames)
	{
		vkDestroyFence(GraphicsDevice->GetHandle(), Frame.InFlightFence, GraphicsApi->GetGraphicsContext()->GetAllocator());
		vkDestroySemaphore(GraphicsDevice->GetHandle(), Frame.RenderFinishedSemaphore, GraphicsApi->GetGraphicsContext()->GetAllocator());
		vkDestroySemaphore(GraphicsDevice->GetHandle(), Frame.ImageAvailableSemaphore, GraphicsApi->GetGraphicsContext()->GetAllocator());
		vkFreeCommandBuffers(GraphicsDevice->GetHandle(), Frame.CommandPool, 1, &Frame.CommandBuffer);
		vkDestroyCommandPool(GraphicsDevice->GetHandle(), Frame.CommandPool, GraphicsApi->GetGraphicsContext()->GetAllocator());
	}
	GraphicsSwapchain->Destroy();
	GraphicsDevice->Destroy();
	GraphicsApi->DestroySurface(Surface);
	GraphicsApi->Destroy();
}

void FRenderer::Resize() const
{
	GraphicsDevice->WaitForIdle();

	GraphicsSwapchain->Resize();
}

void FRenderer::BeginFrame()
{
	FGraphicsFrame& Frame = Frames.at(CurrentFrameId);

	FVulkanResult Result = vkWaitForFences(GraphicsDevice->GetHandle(), 1, &Frame.InFlightFence, VK_TRUE, UINT64_MAX);
	VERIFY(Result == VK_SUCCESS, "Failed to wait for in-flight fence");
	Result = vkResetFences(GraphicsDevice->GetHandle(), 1, &Frame.InFlightFence);
	VERIFY(Result == VK_SUCCESS, "Failed to reset in-flight fence");

	if (!GraphicsSwapchain->AcquireNextImageIndex(Frame.ImageAvailableSemaphore))
	{
		if (Description.Window->IsValid())
			Resize();
	}
	Frame.ImageIndex = GraphicsSwapchain->GetCurrentImageIndex();

	const VkCommandBuffer CommandBuffer = Frame.CommandBuffer;
	vkResetCommandBuffer(CommandBuffer, 0);

	VkCommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = nullptr;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CommandBufferBeginInfo.pInheritanceInfo = nullptr;
	vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);

	VkImageSubresourceRange SubImage;
	SubImage.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	SubImage.baseMipLevel = 0;
	SubImage.levelCount = VK_REMAINING_MIP_LEVELS;
	SubImage.baseArrayLayer = 0;
	SubImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

	VkImageMemoryBarrier2 ImageBarrier = {};
	ImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	ImageBarrier.pNext = nullptr;
	ImageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	ImageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
	ImageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ImageBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	ImageBarrier.subresourceRange = SubImage;
	ImageBarrier.image = GraphicsSwapchain->GetBackBuffer(Frame.ImageIndex).Image;

	VkDependencyInfo DependencyInfo = {};
	DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	DependencyInfo.pNext = nullptr;
	DependencyInfo.imageMemoryBarrierCount = 1;
	DependencyInfo.pImageMemoryBarriers = &ImageBarrier;
	vkCmdPipelineBarrier2(CommandBuffer, &DependencyInfo);
}

void FRenderer::EndFrame() const
{
	const FGraphicsFrame& Frame = Frames.at(CurrentFrameId);
	const VkCommandBuffer CommandBuffer = Frame.CommandBuffer;

	VkImageSubresourceRange SubImage;
	SubImage.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	SubImage.baseMipLevel = 0;
	SubImage.levelCount = VK_REMAINING_MIP_LEVELS;
	SubImage.baseArrayLayer = 0;
	SubImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

	VkImageMemoryBarrier2 ImageBarrier = {};
	ImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	ImageBarrier.pNext = nullptr;
	ImageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	ImageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	ImageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
	ImageBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
	ImageBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	ImageBarrier.subresourceRange = SubImage;
	ImageBarrier.image = GraphicsSwapchain->GetBackBuffer(Frame.ImageIndex).Image;

	VkDependencyInfo DependencyInfo = {};
	DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	DependencyInfo.pNext = nullptr;
	DependencyInfo.imageMemoryBarrierCount = 1;
	DependencyInfo.pImageMemoryBarriers = &ImageBarrier;
	vkCmdPipelineBarrier2(CommandBuffer, &DependencyInfo);

	vkEndCommandBuffer(CommandBuffer);

	VkCommandBufferSubmitInfo CommandBufferSubmitInfo = {};
	CommandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	CommandBufferSubmitInfo.pNext = nullptr;
	CommandBufferSubmitInfo.commandBuffer = Frame.CommandBuffer;

	VkSemaphoreSubmitInfo ImageAvailableSemaphoreSubmitInfo = {};
	ImageAvailableSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	ImageAvailableSemaphoreSubmitInfo.pNext = nullptr;
	ImageAvailableSemaphoreSubmitInfo.semaphore = Frame.ImageAvailableSemaphore;
	ImageAvailableSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;
	ImageAvailableSemaphoreSubmitInfo.value = 0;

	VkSemaphoreSubmitInfo RenderFinishedSemaphoreSubmitInfo = {};
	RenderFinishedSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	RenderFinishedSemaphoreSubmitInfo.pNext = nullptr;
	RenderFinishedSemaphoreSubmitInfo.semaphore = Frame.RenderFinishedSemaphore;
	RenderFinishedSemaphoreSubmitInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
	RenderFinishedSemaphoreSubmitInfo.value = 0;

	VkSubmitInfo2 SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	SubmitInfo.pNext = nullptr;
	SubmitInfo.waitSemaphoreInfoCount = 1;
	SubmitInfo.pWaitSemaphoreInfos = &ImageAvailableSemaphoreSubmitInfo;
	SubmitInfo.signalSemaphoreInfoCount = 1;
	SubmitInfo.pSignalSemaphoreInfos = &RenderFinishedSemaphoreSubmitInfo;
	SubmitInfo.commandBufferInfoCount = 1;
	SubmitInfo.pCommandBufferInfos = &CommandBufferSubmitInfo;
	const FVulkanResult Result = vkQueueSubmit2(GraphicsDevice->GetGraphicsQueue(), 1, &SubmitInfo, Frame.InFlightFence);
	VERIFY(Result == VK_SUCCESS, "Failed to submit command buffer");

	if (!GraphicsSwapchain->Present(Frame.RenderFinishedSemaphore))
	{
		if (Description.Window->IsValid())
			Resize();
	}
}
