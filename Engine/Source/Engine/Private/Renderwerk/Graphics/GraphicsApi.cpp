#include "pch.h"

#include "Renderwerk/Graphics/GraphicsApi.h"

#include "Renderwerk/Platform/Window.h"

#define VOLK_IMPLEMENTATION
#include <volk.h>

FGraphicsApi::FGraphicsApi() = default;

FGraphicsApi::~FGraphicsApi() = default;

void FGraphicsApi::Initialize()
{
	GraphicsContext = MakeShared<FGraphicsContext>();
	GraphicsContext->Initialize();
}

void FGraphicsApi::Destroy() const
{
	GraphicsContext->Destroy();
}

VkSurfaceKHR FGraphicsApi::CreateSurface(const TSharedPtr<FWindow>& Window) const
{
	VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	SurfaceCreateInfo.hwnd = Window->GetHandle();

	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	const FVulkanResult Result = vkCreateWin32SurfaceKHR(GraphicsContext->GetInstance(), &SurfaceCreateInfo, GraphicsContext->GetAllocator(), &Surface);
	ASSERT(Result == VK_SUCCESS, "Failed to create surface");
	return Surface;
}

TSharedPtr<FGraphicsAdapter> FGraphicsApi::ChooseCompatibleAdapter(const VkSurfaceKHR& Surface) const
{
	uint32 AdapterCount = 0;
	FVulkanResult Result = vkEnumeratePhysicalDevices(GraphicsContext->GetInstance(), &AdapterCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate physical devices");
	TVector<VkPhysicalDevice> PhysicalDevices(AdapterCount);
	Result = vkEnumeratePhysicalDevices(GraphicsContext->GetInstance(), &AdapterCount, PhysicalDevices.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate physical devices");
	for (VkPhysicalDevice PhysicalDevice : PhysicalDevices)
	{
		TSharedPtr<FGraphicsAdapter> Adapter = MakeShared<FGraphicsAdapter>();
		Adapter->Initialize(GraphicsContext, PhysicalDevice, Surface);
		if (Adapter->GetType() != EGraphicsAdapterType::Discrete)
			continue;
		if (!Adapter->SupportsExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
			continue;
		return Adapter;
	}
	ASSERT(false, "Failed to find compatible adapter");
	return {};
}

TSharedPtr<FGraphicsDevice> FGraphicsApi::CreateDevice(const TSharedPtr<FGraphicsAdapter>& Adapter) const
{
	TSharedPtr<FGraphicsDevice> GraphicsDevice = MakeShared<FGraphicsDevice>();
	GraphicsDevice->Initialize(GraphicsContext, Adapter);
	return GraphicsDevice;
}

TSharedPtr<FGraphicsSwapchain> FGraphicsApi::CreateSwapchain(const TSharedPtr<FGraphicsDevice>& Device) const
{
	return MakeShared<FGraphicsSwapchain>(GraphicsContext, Device);
}

TSharedPtr<FGraphicsCommandBuffer> FGraphicsApi::AllocateCommandBuffer(const TSharedPtr<FGraphicsDevice>& Device, const VkCommandPool CommandPool) const
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo;
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.pNext = nullptr;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
	const FVulkanResult Result = vkAllocateCommandBuffers(Device->GetHandle(), &CommandBufferAllocateInfo, &CommandBuffer);
	VERIFY(Result == VK_SUCCESS, "Failed to allocate command buffer");

	return MakeShared<FGraphicsCommandBuffer>(CommandBuffer);
}

void FGraphicsApi::SubmitQueue(const VkQueue Queue, const FGraphicsFrame& Frame)
{
	PROFILE_FUNCTION();
	VkCommandBufferSubmitInfo CommandBufferSubmitInfo = {};
	CommandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	CommandBufferSubmitInfo.pNext = nullptr;
	CommandBufferSubmitInfo.commandBuffer = Frame.CommandBuffer->GetHandle();

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
	const FVulkanResult Result = vkQueueSubmit2(Queue, 1, &SubmitInfo, Frame.InFlightFence);
	VERIFY(Result == VK_SUCCESS, "Failed to submit command buffer");
}

void FGraphicsApi::DestroySurface(const VkSurfaceKHR& Surface) const
{
	vkDestroySurfaceKHR(GraphicsContext->GetInstance(), Surface, GraphicsContext->GetAllocator());
}

VkShaderModule FGraphicsApi::CreateShaderModule(const TSharedPtr<FGraphicsDevice>& Device, const TVector<uint32>& SourceContent) const
{
	VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
	ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ShaderModuleCreateInfo.pNext = nullptr;
	ShaderModuleCreateInfo.codeSize = SourceContent.size() * sizeof(uint32);
	ShaderModuleCreateInfo.pCode = SourceContent.data();

	VkShaderModule ShaderModule = VK_NULL_HANDLE;
	const FVulkanResult Result = vkCreateShaderModule(Device->GetHandle(), &ShaderModuleCreateInfo, GraphicsContext->GetAllocator(), &ShaderModule);
	ASSERT(Result == VK_SUCCESS, "Failed to create shader module");
	return ShaderModule;
}

void FGraphicsApi::DeallocateCommandBuffer(const TSharedPtr<FGraphicsDevice>& Device, const VkCommandPool CommandPool,
                                           const TSharedPtr<FGraphicsCommandBuffer>& CommandBuffer) const
{
	VkCommandBuffer CommandBuffers[] = {CommandBuffer->GetHandle()};
	vkFreeCommandBuffers(Device->GetHandle(), CommandPool, _countof(CommandBuffers), CommandBuffers);
}
