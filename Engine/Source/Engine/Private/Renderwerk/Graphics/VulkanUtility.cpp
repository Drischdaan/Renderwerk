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
