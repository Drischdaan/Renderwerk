#include "pch.h"

#include "Renderwerk/Graphics/VulkanGraphicsApi.h"

#include "Renderwerk/Graphics/VulkanGraphicsAdapter.h"
#include "Renderwerk/Graphics/VulkanGraphicsDevice.h"
#include "Renderwerk/Graphics/VulkanGraphicsSwapchain.h"
#include "Renderwerk/Platform/Window.h"

#include <vulkan/vulkan_win32.h>

DEFINE_LOG_CHANNEL(LogVulkan);

namespace
{
#ifdef RW_ENABLE_GRAPHICS_VALIDATION
	VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT Severity, VkDebugUtilsMessageTypeFlagsEXT Type,
	                                                  const VkDebugUtilsMessengerCallbackDataEXT* CallbackData, void* UserData)
	{
		if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			RW_LOG(LogVulkan, Error, "{}", CallbackData->pMessage);
		else if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			RW_LOG(LogVulkan, Warning, "{}", CallbackData->pMessage);
		else if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			RW_LOG(LogVulkan, Info, "{}", CallbackData->pMessage);
		else if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			RW_LOG(LogVulkan, Trace, "{}", CallbackData->pMessage);
		return VK_FALSE;
	}
#endif
}

FVulkanGraphicsApi::FVulkanGraphicsApi(const FVulkanGraphicsApiDesc& InDescription)
	: Description(InDescription)
{
	Context = FVulkanContext();
	AcquireApiVersion();
	CreateAllocator();
	CreateInstance();
#ifdef RW_ENABLE_GRAPHICS_VALIDATION
	CreateDebugMessenger();
#endif
	CreateSurface();
	CreateDevice();
	CreateSwapchain();
	CreateFrames();
}

FVulkanGraphicsApi::~FVulkanGraphicsApi()
{
	Context.GraphicsDevice->WaitForIdle();

	for (const FGraphicsFrame& Frame : Frames)
	{
		vkDestroyFence(Context.GraphicsDevice->Device, Frame.InFlightFence, Context.Allocator);
		vkDestroySemaphore(Context.GraphicsDevice->Device, Frame.RenderFinishedSemaphore, Context.Allocator);
		vkDestroySemaphore(Context.GraphicsDevice->Device, Frame.ImageAvailableSemaphore, Context.Allocator);
		vkFreeCommandBuffers(Context.GraphicsDevice->Device, Frame.CommandPool, 1, &Frame.CommandBuffer);
		vkDestroyCommandPool(Context.GraphicsDevice->Device, Frame.CommandPool, Context.Allocator);
	}
	Swapchain.reset();
	Context.GraphicsDevice.reset();
	vkDestroySurfaceKHR(Context.Instance, Context.Surface, Context.Allocator);
#ifdef RW_ENABLE_GRAPHICS_VALIDATION
	const PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(Context.Instance, "vkDestroyDebugUtilsMessengerEXT"));
	VERIFY(vkDestroyDebugUtilsMessengerEXT != nullptr, "Failed to get vkDestroyDebugUtilsMessengerEXT function");
	vkDestroyDebugUtilsMessengerEXT(Context.Instance, DebugMessenger, Context.Allocator);
#endif
	vkDestroyInstance(Context.Instance, Context.Allocator);
	FMemory::Delete(Context.Allocator);
}

TVector<TSharedPtr<FVulkanGraphicsAdapter>> FVulkanGraphicsApi::AcquireAdapters() const
{
	uint32 PhysicalDeviceCount = 0;
	FVulkanResult Result = vkEnumeratePhysicalDevices(Context.Instance, &PhysicalDeviceCount, nullptr);
	VERIFY(Result == VK_SUCCESS, "Failed to enumerate Vulkan physical devices");
	TVector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	Result = vkEnumeratePhysicalDevices(Context.Instance, &PhysicalDeviceCount, PhysicalDevices.data());
	VERIFY(Result == VK_SUCCESS, "Failed to enumerate Vulkan physical devices");
	TVector<TSharedPtr<FVulkanGraphicsAdapter>> Adapters;
	Adapters.reserve(PhysicalDeviceCount);
	for (const VkPhysicalDevice& PhysicalDevice : PhysicalDevices)
		Adapters.push_back(MakeShared<FVulkanGraphicsAdapter>(Context, PhysicalDevice));
	return Adapters;
}

TSharedPtr<FVulkanGraphicsAdapter> FVulkanGraphicsApi::GetSuitableAdapter() const
{
	const TVector<TSharedPtr<FVulkanGraphicsAdapter>> Adapters = AcquireAdapters();
	TSharedPtr<FVulkanGraphicsAdapter> SuitableAdapter = nullptr;
	for (const TSharedPtr<FVulkanGraphicsAdapter>& Adapter : Adapters)
	{
		if (Adapter->GetProperties().apiVersion < REQUIRED_VULKAN_INSTANCE_VERSION)
			continue;
		if (Adapter->GetProperties().deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			continue;
		SuitableAdapter = Adapter;
	}
	return SuitableAdapter;
}

void FVulkanGraphicsApi::Resize() const
{
	Context.GraphicsDevice->WaitForIdle();
	Swapchain->Resize();
}

void FVulkanGraphicsApi::BeginFrame() const
{
	const FGraphicsFrame& Frame = Frames.at(CurrentFrameId);

	FVulkanResult Result = vkWaitForFences(Context.GraphicsDevice->Device, 1, &Frame.InFlightFence, VK_TRUE, UINT64_MAX);
	VERIFY(Result == VK_SUCCESS, "Failed to wait for in-flight fence");
	Result = vkResetFences(Context.GraphicsDevice->Device, 1, &Frame.InFlightFence);
	VERIFY(Result == VK_SUCCESS, "Failed to reset in-flight fence");

	if (!Swapchain->AcquireImageIndex(Frame.ImageAvailableSemaphore))
	{
		if (Description.Window->IsValid())
			Resize();
		return;
	}

	Result = vkResetCommandBuffer(Frame.CommandBuffer, 0);
	VERIFY(Result == VK_SUCCESS, "Failed to reset command buffer");

	VkCommandBufferBeginInfo CommandBufferBeginInfo;
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferBeginInfo.pNext = nullptr;
	CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	CommandBufferBeginInfo.pInheritanceInfo = nullptr;
	Result = vkBeginCommandBuffer(Frame.CommandBuffer, &CommandBufferBeginInfo);
	VERIFY(Result == VK_SUCCESS, "Failed to begin command buffer");

	const PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(
		vkGetDeviceProcAddr(Context.GraphicsDevice->Device, "vkCmdBeginDebugUtilsLabelEXT"));

	VkDebugUtilsLabelEXT Label = {};
	Label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	Label.pNext = nullptr;
	Label.pLabelName = "Clear Screen";
	Label.color[0] = 1.0f;
	Label.color[1] = 1.0f;
	Label.color[2] = 1.0f;
	Label.color[3] = 1.0f;
	vkCmdBeginDebugUtilsLabelEXT(Frame.CommandBuffer, &Label);
	TransitionImage(Swapchain->GetBackBuffer(Swapchain->GetCurrentImageIndex()), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	constexpr VkClearColorValue ClearValue = {0.0f, 0.0f, 0.0f, 1.0f};
	VkImageSubresourceRange ClearRange;
	ClearRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ClearRange.baseMipLevel = 0;
	ClearRange.levelCount = VK_REMAINING_MIP_LEVELS;
	ClearRange.baseArrayLayer = 0;
	ClearRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	vkCmdClearColorImage(Frame.CommandBuffer, Swapchain->GetBackBuffer(Swapchain->GetCurrentImageIndex()), VK_IMAGE_LAYOUT_GENERAL, &ClearValue, 1, &ClearRange);
}

void FVulkanGraphicsApi::EndFrame()
{
	const FGraphicsFrame& Frame = Frames.at(CurrentFrameId);

	TransitionImage(Swapchain->GetBackBuffer(Swapchain->GetCurrentImageIndex()), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	const PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(
		vkGetDeviceProcAddr(Context.GraphicsDevice->Device, "vkCmdEndDebugUtilsLabelEXT"));
	vkCmdEndDebugUtilsLabelEXT(Frame.CommandBuffer);

	FVulkanResult Result = vkEndCommandBuffer(Frame.CommandBuffer);
	VERIFY(Result == VK_SUCCESS, "Failed to end command buffer");

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
	Result = vkQueueSubmit2(Context.GraphicsDevice->GetGraphicsQueue(), 1, &SubmitInfo, Frame.InFlightFence);
	VERIFY(Result == VK_SUCCESS, "Failed to submit command buffer");

	if (!Swapchain->Present(Frame.RenderFinishedSemaphore))
	{
		if (Description.Window->IsValid())
			Resize();
		return;
	}
	CurrentFrameId = (CurrentFrameId + 1) % Frames.size();
}

void FVulkanGraphicsApi::AcquireApiVersion()
{
	const FVulkanResult Result = vkEnumerateInstanceVersion(&ApiVersion);
	VERIFY(Result == VK_SUCCESS, "Failed to acquire Vulkan API version");

	VERIFY(VK_VERSION_MAJOR(ApiVersion) >= VK_VERSION_MAJOR(REQUIRED_VULKAN_INSTANCE_VERSION), "Vulkan API version is too low");
	VERIFY(VK_VERSION_MINOR(ApiVersion) >= VK_VERSION_MINOR(REQUIRED_VULKAN_INSTANCE_VERSION), "Vulkan API version is too low");
	RW_LOG(LogVulkan, Info, "Vulkan API version: {}.{}.{}", VK_VERSION_MAJOR(ApiVersion), VK_VERSION_MINOR(ApiVersion), VK_VERSION_PATCH(ApiVersion));
}

void FVulkanGraphicsApi::CreateAllocator()
{
	Context.Allocator = FMemory::New<VkAllocationCallbacks>();
	Context.Allocator->pfnAllocation = [](void* UserData, const size64 Size, const size64 Alignment, VkSystemAllocationScope AllocationScope) -> void* {
		return FMemory::Allocate(Size, Alignment);
	};
	Context.Allocator->pfnFree = [](void* UserData, void* Pointer) -> void
	{
		if (Pointer == nullptr)
			return;
		FMemory::Free(Pointer);
	};
	Context.Allocator->pfnReallocation = [](void* UserData, void* OriginalPointer, const size64 Size, const size64 Alignment,
	                                        VkSystemAllocationScope AllocationScope) -> void* {
		return FMemory::Reallocate(OriginalPointer, Size, Alignment);
	};
}

void FVulkanGraphicsApi::CreateInstance()
{
	VkApplicationInfo ApplicationInfo;
	ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ApplicationInfo.pNext = nullptr;
	ApplicationInfo.pApplicationName = "Renderwerk";
	ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	ApplicationInfo.pEngineName = "Renderwerk";
	ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	ApplicationInfo.apiVersion = VK_MAKE_VERSION(VK_VERSION_MAJOR(ApiVersion), VK_VERSION_MINOR(ApiVersion), 0);

	const TVector<const char*> RequiredExtensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#ifdef RW_ENABLE_GRAPHICS_VALIDATION
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
	};
	CheckExtensionAvailability(RequiredExtensions);

	const TVector<const char*> RequiredInstanceLayers = {
#ifdef RW_ENABLE_GRAPHICS_VALIDATION
		"VK_LAYER_KHRONOS_validation",
#endif
	};
	CheckLayerAvailability(RequiredInstanceLayers);

	VkInstanceCreateInfo InstanceCreateInfo;
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pNext = nullptr;
	InstanceCreateInfo.flags = 0;
	InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
	InstanceCreateInfo.enabledLayerCount = static_cast<uint32>(RequiredInstanceLayers.size());
	InstanceCreateInfo.ppEnabledLayerNames = RequiredInstanceLayers.data();
	InstanceCreateInfo.enabledExtensionCount = static_cast<uint32>(RequiredExtensions.size());
	InstanceCreateInfo.ppEnabledExtensionNames = RequiredExtensions.data();

	const FVulkanResult Result = vkCreateInstance(&InstanceCreateInfo, Context.Allocator, &Context.Instance);
	VERIFY(Result == VK_SUCCESS, "Failed to create Vulkan instance");
}

#ifdef RW_ENABLE_GRAPHICS_VALIDATION
void FVulkanGraphicsApi::CreateDebugMessenger()
{
	const PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(Context.Instance, "vkCreateDebugUtilsMessengerEXT"));
	VERIFY(vkCreateDebugUtilsMessengerEXT != nullptr, "Failed to get vkCreateDebugUtilsMessengerEXT function");

	VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo = {};
	DebugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	DebugMessengerCreateInfo.pNext = nullptr;
	DebugMessengerCreateInfo.flags = 0;
	DebugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	DebugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	DebugMessengerCreateInfo.pfnUserCallback = ValidationCallback;
	const FVulkanResult Result = vkCreateDebugUtilsMessengerEXT(Context.Instance, &DebugMessengerCreateInfo, Context.Allocator, &DebugMessenger);
	VERIFY(Result == VK_SUCCESS, "Failed to create Vulkan debug messenger");
}
#endif


void FVulkanGraphicsApi::CreateSurface()
{
	VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo;
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.pNext = nullptr;
	SurfaceCreateInfo.flags = 0;
	SurfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	SurfaceCreateInfo.hwnd = Description.Window->GetHandle();
	const FVulkanResult Result = vkCreateWin32SurfaceKHR(Context.Instance, &SurfaceCreateInfo, Context.Allocator, &Context.Surface);
	VERIFY(Result == VK_SUCCESS, "Failed to create Vulkan surface");
}

void FVulkanGraphicsApi::CreateDevice()
{
	const TSharedPtr<FVulkanGraphicsAdapter> Adapter = GetSuitableAdapter();
	VERIFY(Adapter, "Failed to choose suitable adapter");
	RW_LOG(LogVulkan, Info, "Selected adapter:");
	RW_LOG(LogVulkan, Info, "\t- Name: {}", Adapter->GetProperties().deviceName);
	RW_LOG(LogVulkan, Info, "\t- API version: {}.{}.{}", VK_VERSION_MAJOR(Adapter->GetProperties().apiVersion), VK_VERSION_MINOR(Adapter->GetProperties().apiVersion),
	       VK_VERSION_PATCH(Adapter->GetProperties().apiVersion));
	const uint32 Version = Adapter->GetProperties().driverVersion;
	RW_LOG(LogVulkan, Info, "\t- Driver version: {}.{}.{}.{}", (Version >> 22) & 0x3ff, (Version >> 14) & 0x0ff, (Version >> 6) & 0x0ff, Version & 0x003f);
	Context.GraphicsDevice = MakeShared<FVulkanGraphicsDevice>(Context, Adapter);
}

void FVulkanGraphicsApi::CreateSwapchain()
{
	FVulkanSwapchainDesc SwapchainDesc = {};
	Swapchain = MakeShared<FVulkanGraphicsSwapchain>(Context, SwapchainDesc);
}

void FVulkanGraphicsApi::CreateFrames()
{
	for (FGraphicsFrame& Frame : Frames)
	{
		VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
		CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		CommandPoolCreateInfo.pNext = nullptr;
		CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		CommandPoolCreateInfo.queueFamilyIndex = Context.GraphicsDevice->GetAdapter()->GetQueueMetadata().GraphicsIndex;
		FVulkanResult Result = vkCreateCommandPool(Context.GraphicsDevice->Device, &CommandPoolCreateInfo, Context.Allocator, &Frame.CommandPool);
		VERIFY(Result == VK_SUCCESS, "Failed to create command pool");

		VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
		CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		CommandBufferAllocateInfo.pNext = nullptr;
		CommandBufferAllocateInfo.commandPool = Frame.CommandPool;
		CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		CommandBufferAllocateInfo.commandBufferCount = 1;
		Result = vkAllocateCommandBuffers(Context.GraphicsDevice->Device, &CommandBufferAllocateInfo, &Frame.CommandBuffer);

		VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
		SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		SemaphoreCreateInfo.pNext = nullptr;
		SemaphoreCreateInfo.flags = 0;

		Result = vkCreateSemaphore(Context.GraphicsDevice->Device, &SemaphoreCreateInfo, Context.Allocator, &Frame.ImageAvailableSemaphore);
		VERIFY(Result == VK_SUCCESS, "Failed to create image available semaphore");

		Result = vkCreateSemaphore(Context.GraphicsDevice->Device, &SemaphoreCreateInfo, Context.Allocator, &Frame.RenderFinishedSemaphore);
		VERIFY(Result == VK_SUCCESS, "Failed to create render finished semaphore");

		VkFenceCreateInfo FenceCreateInfo = {};
		FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		FenceCreateInfo.pNext = nullptr;
		FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		Result = vkCreateFence(Context.GraphicsDevice->Device, &FenceCreateInfo, Context.Allocator, &Frame.InFlightFence);
		VERIFY(Result == VK_SUCCESS, "Failed to create in-flight fence");
	}
}

void FVulkanGraphicsApi::TransitionImage(const VkImage Image, const VkImageLayout CurrentLayout, const VkImageLayout NewLayout) const
{
	const FGraphicsFrame& Frame = Frames.at(CurrentFrameId);

	VkImageSubresourceRange SubImage;
	SubImage.aspectMask = (NewLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
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
	ImageBarrier.oldLayout = CurrentLayout;
	ImageBarrier.newLayout = NewLayout;
	ImageBarrier.subresourceRange = SubImage;
	ImageBarrier.image = Image;

	VkDependencyInfo DependencyInfo = {};
	DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	DependencyInfo.pNext = nullptr;
	DependencyInfo.imageMemoryBarrierCount = 1;
	DependencyInfo.pImageMemoryBarriers = &ImageBarrier;
	vkCmdPipelineBarrier2(Frame.CommandBuffer, &DependencyInfo);
}

void FVulkanGraphicsApi::CheckExtensionAvailability(const TVector<const char*>& RequiredExtensions)
{
	uint32 AvailableExtensionCount = 0;
	FVulkanResult Result = vkEnumerateInstanceExtensionProperties(nullptr, &AvailableExtensionCount, nullptr);
	VERIFY(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance extensions");
	TVector<VkExtensionProperties> AvailableExtensions(AvailableExtensionCount);
	Result = vkEnumerateInstanceExtensionProperties(nullptr, &AvailableExtensionCount, AvailableExtensions.data());
	VERIFY(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance extensions");
	for (const char* RequiredExtension : RequiredExtensions)
	{
		VERIFY(IsExtensionSupported(RequiredExtension, AvailableExtensions), "Required extension '{}' is not supported", RequiredExtension);
	}
}

bool8 FVulkanGraphicsApi::IsExtensionSupported(const char* ExtensionName, const TVector<VkExtensionProperties>& AvailableExtensions)
{
	return std::ranges::any_of(AvailableExtensions, [ExtensionName](const VkExtensionProperties& Extension)
	{
		return strcmp(Extension.extensionName, ExtensionName) == 0;
	});
}

void FVulkanGraphicsApi::CheckLayerAvailability(const TVector<const char*>& RequiredLayers)
{
	uint32 AvailableLayerCount = 0;
	FVulkanResult Result = vkEnumerateInstanceLayerProperties(&AvailableLayerCount, nullptr);
	VERIFY(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance layers");
	TVector<VkLayerProperties> AvailableLayers(AvailableLayerCount);
	Result = vkEnumerateInstanceLayerProperties(&AvailableLayerCount, AvailableLayers.data());
	VERIFY(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance layers");
	for (const char* RequiredLayer : RequiredLayers)
	{
		VERIFY(IsLayerSupported(RequiredLayer, AvailableLayers), "Required layer '{}' is not supported", RequiredLayer);
	}
}

bool8 FVulkanGraphicsApi::IsLayerSupported(const char* LayerName, const TVector<VkLayerProperties>& AvailableLayers)
{
	return std::ranges::any_of(AvailableLayers, [LayerName](const VkLayerProperties& Extension)
	{
		return strcmp(Extension.layerName, LayerName) == 0;
	});
}
