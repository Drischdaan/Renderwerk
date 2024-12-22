#include "pch.h"

#include "Renderwerk/Graphics/VulkanUtility.h"

uint32 VulkanUtility::GetInstanceVersion()
{
	uint32 ApiVersion = VK_API_VERSION_1_0;
	const VkResult Result = vkEnumerateInstanceVersion(&ApiVersion);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance version");
	return ApiVersion;
}

TVector<FString> VulkanUtility::GetInstanceLayers()
{
	uint32 LayerCount = 0;
	VkResult Result = vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance layers");
	TVector<VkLayerProperties> Layers(LayerCount);
	Result = vkEnumerateInstanceLayerProperties(&LayerCount, Layers.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance layers");
	TVector<FString> LayerNames;
	std::ranges::transform(Layers, std::back_inserter(LayerNames), [](const VkLayerProperties& Layer) { return FString(Layer.layerName); });
	return LayerNames;
}

TVector<FString> VulkanUtility::GetInstanceExtensions()
{
	uint32 ExtensionCount = 0;
	VkResult Result = vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance extensions");
	TVector<VkExtensionProperties> Extensions(ExtensionCount);
	Result = vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, Extensions.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan instance extensions");
	TVector<FString> ExtensionNames;
	std::ranges::transform(Extensions, std::back_inserter(ExtensionNames), [](const VkExtensionProperties& Extension) { return FString(Extension.extensionName); });
	return ExtensionNames;
}

TVector<FString> VulkanUtility::GetPhysicalDeviceLayers(const VkPhysicalDevice PhysicalDevice)
{
	uint32 LayerCount = 0;
	VkResult Result = vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan physical device layers");
	TVector<VkLayerProperties> Layers(LayerCount);
	Result = vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, Layers.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan physical device layers");
	TVector<FString> LayerNames;
	std::ranges::transform(Layers, std::back_inserter(LayerNames), [](const VkLayerProperties& Layer) { return FString(Layer.layerName); });
	return LayerNames;
}

TVector<FString> VulkanUtility::GetPhysicalDeviceExtensions(const VkPhysicalDevice PhysicalDevice)
{
	uint32 ExtensionCount = 0;
	VkResult Result = vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan physical device extensions");
	TVector<VkExtensionProperties> Extensions(ExtensionCount);
	Result = vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, Extensions.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan physical device extensions");
	TVector<FString> ExtensionNames;
	std::ranges::transform(Extensions, std::back_inserter(ExtensionNames), [](const VkExtensionProperties& Extension) { return FString(Extension.extensionName); });
	return ExtensionNames;
}

TVector<VkPhysicalDevice> VulkanUtility::GetPhysicalDevices(const VkInstance Instance)
{
	uint32 DeviceCount = 0;
	VkResult Result = vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan physical devices");
	TVector<VkPhysicalDevice> PhysicalDevices(DeviceCount);
	Result = vkEnumeratePhysicalDevices(Instance, &DeviceCount, PhysicalDevices.data());
	ASSERT(Result == VK_SUCCESS, "Failed to enumerate Vulkan physical devices");
	return PhysicalDevices;
}

TVector<VkQueueFamilyProperties> VulkanUtility::GetQueueFamilyProperties(const VkPhysicalDevice PhysicalDevice)
{
	uint32 QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
	TVector<VkQueueFamilyProperties> QueueFamilyProperties(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.data());
	return QueueFamilyProperties;
}

TVector<VkSurfaceFormatKHR> VulkanUtility::GetSurfaceFormats(const VkPhysicalDevice PhysicalDevice, const VkSurfaceKHR Surface)
{
	uint32 FormatCount = 0;
	VkResult Result = vkGetPhysicalDeviceSurfaceFormatsKHR(VK_NULL_HANDLE, VK_NULL_HANDLE, &FormatCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to get physical device surface formats");
	TVector<VkSurfaceFormatKHR> SurfaceFormats(FormatCount);
	Result = vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, SurfaceFormats.data());
	ASSERT(Result == VK_SUCCESS, "Failed to get physical device surface formats");
	return SurfaceFormats;
}

TVector<VkPresentModeKHR> VulkanUtility::GetPresentModes(const VkPhysicalDevice PhysicalDevice, const VkSurfaceKHR Surface)
{
	uint32 PresentModeCount = 0;
	VkResult Result = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, nullptr);
	ASSERT(Result == VK_SUCCESS, "Failed to get physical device present modes");
	TVector<VkPresentModeKHR> PresentModes(PresentModeCount);
	Result = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, PresentModes.data());
	ASSERT(Result == VK_SUCCESS, "Failed to get physical device present modes");
	return PresentModes;
}
