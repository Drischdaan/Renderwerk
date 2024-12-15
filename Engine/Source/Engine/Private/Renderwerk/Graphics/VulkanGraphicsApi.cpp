#include "pch.h"

#include "Renderwerk/Graphics/VulkanGraphicsApi.h"

#include <vulkan/vulkan_win32.h>

namespace
{
	DEFINE_LOG_CHANNEL(LogVulkan);

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

FVulkanGraphicsApi::FVulkanGraphicsApi()
{
	AcquireApiVersion();
	CreateAllocator();
	CreateInstance();
#ifdef RW_ENABLE_GRAPHICS_VALIDATION
	CreateDebugMessenger();
#endif
}

FVulkanGraphicsApi::~FVulkanGraphicsApi()
{
#ifdef RW_ENABLE_GRAPHICS_VALIDATION
	const PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT"));
	VERIFY(vkDestroyDebugUtilsMessengerEXT != nullptr, "Failed to get vkDestroyDebugUtilsMessengerEXT function");

	vkDestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, Allocator);
#endif
	vkDestroyInstance(Instance, Allocator);
	FMemory::Delete(Allocator);
}

void FVulkanGraphicsApi::AcquireApiVersion()
{
	const VkResult Result = vkEnumerateInstanceVersion(&ApiVersion);
	VERIFY(Result == VK_SUCCESS, "Failed to acquire Vulkan API version");

	VERIFY(VK_VERSION_MAJOR(ApiVersion) >= VK_VERSION_MAJOR(REQUIRED_VULKAN_INSTANCE_VERSION), "Vulkan API version is too low");
	VERIFY(VK_VERSION_MINOR(ApiVersion) >= VK_VERSION_MINOR(REQUIRED_VULKAN_INSTANCE_VERSION), "Vulkan API version is too low");
}

void FVulkanGraphicsApi::CreateAllocator()
{
	Allocator = FMemory::New<VkAllocationCallbacks>();
	Allocator->pfnAllocation = [](void* UserData, const size64 Size, const size64 Alignment, VkSystemAllocationScope AllocationScope) -> void* {
		return FMemory::Allocate(Size, Alignment);
	};
	Allocator->pfnFree = [](void* UserData, void* Pointer) -> void
	{
		if (Pointer == nullptr)
			return;
		FMemory::Free(Pointer);
	};
	Allocator->pfnReallocation = [](void* UserData, void* OriginalPointer, const size64 Size, const size64 Alignment, VkSystemAllocationScope AllocationScope) -> void* {
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
	ApplicationInfo.apiVersion = ApiVersion;

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

	const VkResult Result = vkCreateInstance(&InstanceCreateInfo, Allocator, &Instance);
	VERIFY(Result == VK_SUCCESS, "Failed to create Vulkan instance");
}

#ifdef RW_ENABLE_GRAPHICS_VALIDATION
void FVulkanGraphicsApi::CreateDebugMessenger()
{
	const PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT"));
	VERIFY(vkCreateDebugUtilsMessengerEXT != nullptr, "Failed to get vkCreateDebugUtilsMessengerEXT function");

	VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo = {};
	DebugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	DebugMessengerCreateInfo.pNext = nullptr;
	DebugMessengerCreateInfo.flags = 0;
	DebugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	DebugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	DebugMessengerCreateInfo.pfnUserCallback = ValidationCallback;
	const VkResult Result = vkCreateDebugUtilsMessengerEXT(Instance, &DebugMessengerCreateInfo, Allocator, &DebugMessenger);
	VERIFY(Result == VK_SUCCESS, "Failed to create Vulkan debug messenger");
}
#endif

void FVulkanGraphicsApi::CheckExtensionAvailability(const TVector<const char*>& RequiredExtensions)
{
	uint32 AvailableExtensionCount = 0;
	VkResult Result = vkEnumerateInstanceExtensionProperties(nullptr, &AvailableExtensionCount, nullptr);
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
	VkResult Result = vkEnumerateInstanceLayerProperties(&AvailableLayerCount, nullptr);
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
