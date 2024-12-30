#include "pch.h"

#include "Renderwerk/Renderer/Renderer.h"

#include <glm/gtx/std_based_type.inl>

#include "glm/vec3.hpp"

#include "Renderwerk/Graphics/GraphicsAdapter.h"
#include "Renderwerk/Graphics/GraphicsBackend.h"
#include "Renderwerk/Graphics/GraphicsCommandBuffer.h"
#include "Renderwerk/Graphics/GraphicsCommandPool.h"
#include "Renderwerk/Graphics/GraphicsCommandQueue.h"
#include "Renderwerk/Graphics/GraphicsDevice.h"
#include "Renderwerk/Graphics/GraphicsRenderTarget.h"
#include "Renderwerk/Graphics/GraphicsSwapchain.h"
#include "Renderwerk/Platform/Window.h"
#include "Renderwerk/Scene/Components.h"

DEFINE_LOG_CHANNEL(LogRenderer);

FRenderer::FRenderer() = default;

FRenderer::~FRenderer() = default;

void FRenderer::Initialize(const FRendererDesc& InDescription)
{
	Description = InDescription;

	const FGraphicsBackendDesc BackendDesc = {};
	GraphicsBackend = MakeUnique<FGraphicsBackend>();
	GraphicsBackend->Initialize(BackendDesc);

	Surface = GraphicsBackend->CreateSurface(Description.Window);

	TVector<const char*> DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
	};

	VkPhysicalDeviceBufferDeviceAddressFeatures BufferDeviceAddressFeatures = Vulkan::CreateStructure<VkPhysicalDeviceBufferDeviceAddressFeatures>();
	BufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceDynamicRenderingFeatures DynamicRenderingFeatures = Vulkan::CreateStructure<VkPhysicalDeviceDynamicRenderingFeatures>();
	DynamicRenderingFeatures.pNext = &BufferDeviceAddressFeatures;
	DynamicRenderingFeatures.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceSynchronization2Features Synchronization2Features = Vulkan::CreateStructure<VkPhysicalDeviceSynchronization2Features>();
	Synchronization2Features.pNext = &DynamicRenderingFeatures;
	Synchronization2Features.synchronization2 = VK_TRUE;

	const TSharedPtr<FGraphicsAdapter> GraphicsAdapter = SelectAdapter(DeviceExtensions);
	GraphicsDevice = GraphicsBackend->CreateDevice(GraphicsAdapter);
	GraphicsDevice->Initialize(DeviceExtensions, &Synchronization2Features);

	GraphicsResourceAllocator = GraphicsApi->CreateResourceAllocator(GraphicsDevice);
	GraphicsResourceAllocator->Initialize();

	FGraphicsSwapchainDesc SwapchainDesc = {};
	SwapchainDesc.Surface = Surface;
	GraphicsSwapchain = GraphicsDevice->CreateSwapchain();
	GraphicsSwapchain->Initialize(SwapchainDesc);

	for (FGraphicsFrame& Frame : Frames)
	{
		FGraphicsCommandPoolDesc CommandPoolDesc = {};
		CommandPoolDesc.QueueFamilyIndex = GraphicsAdapter->GetQueueMetadata(EGraphicsQueueType::Graphics).FamilyIndex;
		Frame.CommandPool = GraphicsDevice->CreateCommandPool();
		Frame.CommandPool->Initialize(CommandPoolDesc);

		Frame.MainCommandBuffer = Frame.CommandPool->AllocateBuffer();

		Frame.ImageWaitSemaphore = GraphicsDevice->CreateSemaphore();
		Frame.RenderingFinishedSignalSemaphore = GraphicsDevice->CreateSemaphore();
		Frame.InFlightFence = GraphicsDevice->CreateFence();
	}

	FGraphicsRenderTargetDesc RenderTargetDesc = {};
	RenderTargetDesc.Width = GraphicsSwapchain->GetExtent().width;
	RenderTargetDesc.Height = GraphicsSwapchain->GetExtent().height;
	RenderTargetDesc.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};
	RenderTarget = GraphicsDevice->CreateRenderTarget();
	RenderTarget->Initialize(RenderTargetDesc);
}

void FRenderer::Destroy()
{
	GraphicsDevice->WaitForIdle();

	RenderTarget->Destroy();
	RenderTarget.reset();
	for (FGraphicsFrame& Frame : Frames)
	{
		GraphicsDevice->DestroyFence(Frame.InFlightFence);
		GraphicsDevice->DestroySemaphore(Frame.RenderingFinishedSignalSemaphore);
		GraphicsDevice->DestroySemaphore(Frame.ImageWaitSemaphore);
		Frame.CommandPool->FreeBuffer(Frame.MainCommandBuffer);
		Frame.CommandPool->Destroy();
		Frame.CommandPool.reset();
	}

	GraphicsSwapchain->Destroy();
	GraphicsSwapchain.reset();
	GraphicsResourceAllocator->Destroy();
	GraphicsResourceAllocator.reset();
	GraphicsDevice->Destroy();
	GraphicsDevice.reset();
	GraphicsBackend->DestroySurface(Surface);
	GraphicsBackend->Destroy();
	GraphicsBackend.reset();
}

void FRenderer::Resize()
{
	PROFILE_FUNCTION();
	GraphicsDevice->WaitForIdle();

	RenderTarget->Destroy();
	RenderTarget.reset();

	GraphicsSwapchain->Resize();

	FGraphicsRenderTargetDesc RenderTargetDesc = {};
	RenderTargetDesc.Width = GraphicsSwapchain->GetExtent().width;
	RenderTargetDesc.Height = GraphicsSwapchain->GetExtent().height;
	RenderTargetDesc.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};
	RenderTarget = GraphicsDevice->CreateRenderTarget();
	RenderTarget->Initialize(RenderTargetDesc);
}

void FRenderer::BeginFrame()
{
	PROFILE_FUNCTION();

	const FGraphicsFrame& Frame = Frames.at(FrameIndex);

	{
		PROFILE_SCOPE("vkWaitForFences");
		VkResult Result = vkWaitForFences(GraphicsDevice->GetHandle(), 1, &Frame.InFlightFence, VK_TRUE, UINT64_MAX);
		ASSERT(Result == VK_SUCCESS, "Failed to wait for fence");
		Result = vkResetFences(GraphicsDevice->GetHandle(), 1, &Frame.InFlightFence);
		ASSERT(Result == VK_SUCCESS, "Failed to reset fence");
	}

	if (!GraphicsSwapchain->AcquireNextImageIndex(Frame.ImageWaitSemaphore))
	{
		if (Description.Window->IsValid())
			Resize();
		return;
	}

	const TSharedPtr<FGraphicsCommandBuffer> CommandBuffer = Frame.MainCommandBuffer;
	const FGraphicsBackBuffer BackBuffer = GraphicsSwapchain->GetCurrentBackBuffer();

	CommandBuffer->Reset();
	CommandBuffer->Begin();

	CommandBuffer->TransitionImageLayout(BackBuffer.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	CommandBuffer->TransitionImageLayout(RenderTarget->GetHandle(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	CommandBuffer->BeginDebugLabel("ClearRenderTargets");
	{
		CommandBuffer->ClearImage(BackBuffer.Image, VK_IMAGE_LAYOUT_GENERAL);
		CommandBuffer->ClearRenderTarget(RenderTarget);
	}
	CommandBuffer->EndDebugLabel();
}

void FRenderer::EndFrame()
{
	PROFILE_FUNCTION();

	const FGraphicsFrame& Frame = Frames.at(FrameIndex);
	const TSharedPtr<FGraphicsCommandBuffer> CommandBuffer = Frame.MainCommandBuffer;
	const FGraphicsBackBuffer BackBuffer = GraphicsSwapchain->GetCurrentBackBuffer();

	CommandBuffer->BeginDebugLabel("BlitRenderTarget");
	{
		CommandBuffer->BlitImages(RenderTarget->GetHandle(), BackBuffer.Image, GraphicsSwapchain->GetExtent());
	}
	CommandBuffer->EndDebugLabel();

	CommandBuffer->TransitionImageLayout(BackBuffer.Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	CommandBuffer->End();

	TVector<TSharedPtr<FGraphicsCommandBuffer>> CommandBuffers = {CommandBuffer};
	FGraphicsSubmitSyncDesc SubmitSyncDesc = {};
	SubmitSyncDesc.WaitSemaphores = {Frame.ImageWaitSemaphore};
	SubmitSyncDesc.SignalSemaphores = {Frame.RenderingFinishedSignalSemaphore};
	SubmitSyncDesc.SubmitFence = Frame.InFlightFence;
	GraphicsDevice->GetGraphicsQueue()->SubmitCommandBuffers(CommandBuffers, SubmitSyncDesc);

	if (!GraphicsSwapchain->Present(GraphicsDevice->GetGraphicsQueue(), Frame.RenderingFinishedSignalSemaphore))
	{
		if (Description.Window->IsValid())
			Resize();
		return;
	}
	FrameIndex = (FrameIndex + 1) % RENDERER_FRAME_COUNT;
}

TSharedPtr<FGraphicsAdapter> FRenderer::SelectAdapter(const TSpan<const char*>& RequiredExtensions) const
{
	TSharedPtr<FGraphicsAdapter> GraphicsAdapter;
	const TVector<TSharedPtr<FGraphicsAdapter>> Adapters = GraphicsBackend->GetAdapters();
	for (const TSharedPtr<FGraphicsAdapter>& CurrentAdapter : Adapters)
	{
		CurrentAdapter->Initialize(Surface);
		if (GraphicsBackend->IsAdapterSuitable(CurrentAdapter, RequiredExtensions))
		{
			GraphicsAdapter = CurrentAdapter;
			break;
		}
	}
	ASSERT(GraphicsAdapter, "No suitable adapter found");
	RW_LOG(LogGraphics, Info, "Selected adapter: {}", GraphicsAdapter->GetProperties().Name);
	RW_LOG(LogGraphics, Info, "\t- Type: {}", GetEnumValueName(GraphicsAdapter->GetProperties().Type));
	RW_LOG(LogGraphics, Info, "\t- Vendor: {}", GetVendorString(GraphicsAdapter->GetProperties().Vendor));
	RW_LOG(LogGraphics, Info, "\t- Driver Version: {}", GraphicsAdapter->GetDriverVersionString());
	RW_LOG(LogGraphics, Info, "\t- Api Version: {}.{}.{}", VK_VERSION_MAJOR(GraphicsAdapter->GetProperties().ApiVersion),
	       VK_VERSION_MINOR(GraphicsAdapter->GetProperties().ApiVersion), VK_VERSION_PATCH(GraphicsAdapter->GetProperties().ApiVersion));
	RW_LOG(LogGraphics, Info, "|   Type   | Family Index | Queue Index |");
	RW_LOG(LogGraphics, Info, "|----------|--------------|-------------|");
	for (const auto& [Type, Metadata] : GraphicsAdapter->GetQueueMetadataMap())
	{
		FString QueueName = FString(GetEnumValueName(Type));
		const size64 Padding = 8 - QueueName.length();
		if (Padding > 0)
		{
			for (size64 Index = 0; Index < Padding; ++Index)
				QueueName += " ";
		}
		RW_LOG(LogGraphics, Info, "| {} |       {}      |      {}      |", QueueName, Metadata.FamilyIndex, Metadata.QueueIndex);
	}
	return GraphicsAdapter;
}

void FRenderer::SubmitImmediately(const TFunction<void(VkCommandBuffer)>& Command) const
{
	FVulkanResult Result = vkResetFences(GraphicsDevice->GetHandle(), 1, &ImmediateFence);
	VERIFY(Result == VK_SUCCESS, "Failed to reset fence");

	Result = vkResetCommandBuffer(ImmediateCommandBuffer, 0);
	VERIFY(Result == VK_SUCCESS, "Failed to reset command buffer");

	VkCommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = nullptr;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CommandBufferBeginInfo.pInheritanceInfo = nullptr;
	Result = vkBeginCommandBuffer(ImmediateCommandBuffer, &CommandBufferBeginInfo);
	VERIFY(Result == VK_SUCCESS, "Failed to begin command buffer");

	Command(ImmediateCommandBuffer);

	Result = vkEndCommandBuffer(ImmediateCommandBuffer);
	VERIFY(Result == VK_SUCCESS, "Failed to end command buffer");

	VkCommandBufferSubmitInfo CommandBufferSubmitInfo = {};
	CommandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	CommandBufferSubmitInfo.pNext = nullptr;
	CommandBufferSubmitInfo.commandBuffer = ImmediateCommandBuffer;

	VkSubmitInfo2 SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	SubmitInfo.pNext = nullptr;
	SubmitInfo.commandBufferInfoCount = 1;
	SubmitInfo.pCommandBufferInfos = &CommandBufferSubmitInfo;

	Result = vkQueueSubmit2(GraphicsDevice->GetGraphicsQueue(), 1, &SubmitInfo, ImmediateFence);
	VERIFY(Result == VK_SUCCESS, "Failed to submit command buffer");

	Result = vkWaitForFences(GraphicsDevice->GetHandle(), 1, &ImmediateFence, VK_TRUE, UINT64_MAX);
	VERIFY(Result == VK_SUCCESS, "Failed to wait for fence");
}

void FRenderer::CreateDepthImage()
{
	DepthImageFormat = VK_FORMAT_D32_SFLOAT;

	VkImageCreateInfo ImageCreateInfo = {};
	ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ImageCreateInfo.pNext = nullptr;
	ImageCreateInfo.flags = 0;
	ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	ImageCreateInfo.format = DepthImageFormat;
	ImageCreateInfo.extent.width = GraphicsSwapchain->GetExtent().width;
	ImageCreateInfo.extent.height = GraphicsSwapchain->GetExtent().height;
	ImageCreateInfo.extent.depth = 1;
	ImageCreateInfo.mipLevels = 1;
	ImageCreateInfo.arrayLayers = 1;
	ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	ImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	VmaAllocationCreateInfo AllocationCreateInfo = {};
	AllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	AllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	vmaCreateImage(GraphicsResourceAllocator->GetHandle(), &ImageCreateInfo, &AllocationCreateInfo, &DepthImage, &DepthImageAllocation, nullptr);

	VkImageViewCreateInfo ImageViewCreateInfo;
	ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ImageViewCreateInfo.pNext = nullptr;
	ImageViewCreateInfo.flags = 0;
	ImageViewCreateInfo.image = DepthImage;
	ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ImageViewCreateInfo.format = DepthImageFormat;
	ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	ImageViewCreateInfo.subresourceRange.levelCount = 1;
	ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	ImageViewCreateInfo.subresourceRange.layerCount = 1;

	vkCreateImageView(GraphicsDevice->GetHandle(), &ImageViewCreateInfo, GraphicsApi->GetGraphicsContext()->GetAllocator(), &DepthImageView);
}

void FRenderer::DestroyDepthImage() const
{
	vkDestroyImageView(GraphicsDevice->GetHandle(), DepthImageView, GraphicsApi->GetGraphicsContext()->GetAllocator());
	vmaDestroyImage(GraphicsResourceAllocator->GetHandle(), DepthImage, DepthImageAllocation);
}
