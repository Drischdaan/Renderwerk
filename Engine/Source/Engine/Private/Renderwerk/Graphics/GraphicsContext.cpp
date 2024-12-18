#include "pch.h"

#include "Renderwerk/Graphics/GraphicsContext.h"

#define REQUIRED_VULKAN_INSTANCE_VERSION VK_API_VERSION_1_3

#ifdef RW_ENABLE_GRAPHICS_VALIDATION
namespace
{
	VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT Severity, VkDebugUtilsMessageTypeFlagsEXT Type,
	                                                  const VkDebugUtilsMessengerCallbackDataEXT* CallbackData, void* UserData)
	{
		if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			RW_LOG(LogGraphics, Error, "{}", CallbackData->pMessage);
		else if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			RW_LOG(LogGraphics, Warning, "{}", CallbackData->pMessage);
		else if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			RW_LOG(LogGraphics, Info, "{}", CallbackData->pMessage);
		else if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			RW_LOG(LogGraphics, Trace, "{}", CallbackData->pMessage);
		return VK_FALSE;
	}
}
#endif

FGraphicsContext::FGraphicsContext() = default;

FGraphicsContext::~FGraphicsContext() = default;

void FGraphicsContext::Initialize()
{
	const FVulkanResult Result = volkInitialize();
	ASSERT(Result == VK_SUCCESS, "Failed to initialize Vulkan loader");
	AcquireApiVersion();
	CreateAllocator();
	CreateInstance();
	CreateDebugMessenger();
}

void FGraphicsContext::Destroy() const
{
	vkDestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, &Allocator);
	vkDestroyInstance(Instance, &Allocator);
	volkFinalize();
}

void FGraphicsContext::AcquireApiVersion()
{
	const FVulkanResult Result = vkEnumerateInstanceVersion(&ApiVersion);
	ASSERT(Result == VK_SUCCESS, "Failed to acquire Vulkan instance version");

	ASSERT(VK_VERSION_MAJOR(ApiVersion) >= VK_VERSION_MAJOR(REQUIRED_VULKAN_INSTANCE_VERSION), "Vulkan API version is too low");
	ASSERT(VK_VERSION_MINOR(ApiVersion) >= VK_VERSION_MINOR(REQUIRED_VULKAN_INSTANCE_VERSION), "Vulkan API version is too low");
	RW_LOG(LogGraphics, Info, "Vulkan API version: {}.{}.{}", VK_VERSION_MAJOR(ApiVersion), VK_VERSION_MINOR(ApiVersion), VK_VERSION_PATCH(ApiVersion));
}

void FGraphicsContext::CreateAllocator()
{
	Allocator = {};
	Allocator.pfnAllocation = [](void* UserData, const size64 Size, const size64 Alignment, VkSystemAllocationScope AllocationScope) -> void* {
		return FMemory::Allocate(Size, Alignment);
	};
	Allocator.pfnFree = [](void* UserData, void* Pointer) -> void
	{
		if (Pointer == nullptr)
			return;
		FMemory::Free(Pointer);
	};
	Allocator.pfnReallocation = [](void* UserData, void* OriginalPointer, const size64 Size, const size64 Alignment,
	                               VkSystemAllocationScope AllocationScope) -> void* {
		return FMemory::Reallocate(OriginalPointer, Size, Alignment);
	};
}

void FGraphicsContext::CreateInstance()
{
	const TVector<const char*> RequiredExtensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#ifdef RW_ENABLE_GRAPHICS_VALIDATION
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
	};
	CheckInstanceExtensions(RequiredExtensions);

	const TVector<const char*> RequiredInstanceLayers = {
#ifdef RW_ENABLE_GRAPHICS_VALIDATION
		"VK_LAYER_KHRONOS_validation",
#endif
	};
	CheckInstanceLayers(RequiredInstanceLayers);

	VkApplicationInfo ApplicationInfo;
	ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ApplicationInfo.pNext = nullptr;
	ApplicationInfo.pApplicationName = RW_ENGINE_NAME;
	ApplicationInfo.applicationVersion = VK_MAKE_VERSION(RW_ENGINE_VERSION_MAJOR, RW_ENGINE_VERSION_MINOR, RW_ENGINE_VERSION_PATCH);
	ApplicationInfo.pEngineName = RW_ENGINE_NAME;
	ApplicationInfo.engineVersion = VK_MAKE_VERSION(RW_ENGINE_VERSION_MAJOR, RW_ENGINE_VERSION_MINOR, RW_ENGINE_VERSION_PATCH);
	ApplicationInfo.apiVersion = VK_MAKE_VERSION(VK_VERSION_MAJOR(ApiVersion), VK_VERSION_MINOR(ApiVersion), 0);

	VkInstanceCreateInfo InstanceCreateInfo;
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pNext = nullptr;
	InstanceCreateInfo.flags = 0;
	InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
	InstanceCreateInfo.enabledLayerCount = static_cast<uint32>(RequiredInstanceLayers.size());
	InstanceCreateInfo.ppEnabledLayerNames = RequiredInstanceLayers.data();
	InstanceCreateInfo.enabledExtensionCount = static_cast<uint32>(RequiredExtensions.size());
	InstanceCreateInfo.ppEnabledExtensionNames = RequiredExtensions.data();

	const FVulkanResult Result = vkCreateInstance(&InstanceCreateInfo, &Allocator, &Instance);
	ASSERT(Result == VK_SUCCESS, "Failed to create Vulkan instance");
	volkLoadInstance(Instance);
}

#ifdef RW_ENABLE_GRAPHICS_VALIDATION
void FGraphicsContext::CreateDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo = {};
	DebugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	DebugMessengerCreateInfo.pNext = nullptr;
	DebugMessengerCreateInfo.flags = 0;
	DebugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	DebugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	DebugMessengerCreateInfo.pfnUserCallback = ValidationCallback;
	const FVulkanResult Result = vkCreateDebugUtilsMessengerEXT(Instance, &DebugMessengerCreateInfo, &Allocator, &DebugMessenger);
	ASSERT(Result == VK_SUCCESS, "Failed to create Vulkan debug messenger");
	RW_LOG(LogGraphics, Debug, "Vulkan debug messenger enabled");
}
#endif

void FGraphicsContext::CheckInstanceExtensions(const TVector<const char*>& RequiredExtensions)
{
	uint32 AvailableExtensionCount = 0;
	FVulkanResult Result = vkEnumerateInstanceExtensionProperties(nullptr, &AvailableExtensionCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance extensions");
	TVector<VkExtensionProperties> AvailableExtensions(AvailableExtensionCount);
	Result = vkEnumerateInstanceExtensionProperties(nullptr, &AvailableExtensionCount, AvailableExtensions.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance extensions");
	RW_LOG(LogGraphics, Info, "Enabled instance extensions:");
	std::ranges::for_each(RequiredExtensions, [&](const char* ExtensionName)
	{
		const bool8 bExtensionFound = std::ranges::any_of(AvailableExtensions, [&](const VkExtensionProperties& Extension)
		{
			return strcmp(Extension.extensionName, ExtensionName) == 0;
		});
		ASSERT(bExtensionFound, "Required Vulkan instance extension '{}' not found", ExtensionName);
		RW_LOG(LogGraphics, Info, "\t- {}", ExtensionName);
	});
}

void FGraphicsContext::CheckInstanceLayers(const TVector<const char*>& RequiredExtensions)
{
	uint32 AvailableLayerCount = 0;
	FVulkanResult Result = vkEnumerateInstanceLayerProperties(&AvailableLayerCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance layers");
	TVector<VkLayerProperties> AvailableLayers(AvailableLayerCount);
	Result = vkEnumerateInstanceLayerProperties(&AvailableLayerCount, AvailableLayers.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance layers");
	if (RequiredExtensions.size() > 0)
		RW_LOG(LogGraphics, Info, "Enabled instance layers:");
	std::ranges::for_each(RequiredExtensions, [&](const char* LayerName)
	{
		const bool8 bLayerFound = std::ranges::any_of(AvailableLayers, [&](const VkLayerProperties& Layer)
		{
			return strcmp(Layer.layerName, LayerName) == 0;
		});
		ASSERT(bLayerFound, "Required Vulkan instance layer '{}' not found", LayerName);
		RW_LOG(LogGraphics, Info, "\t- {}", LayerName);
	});
}
