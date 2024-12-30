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
