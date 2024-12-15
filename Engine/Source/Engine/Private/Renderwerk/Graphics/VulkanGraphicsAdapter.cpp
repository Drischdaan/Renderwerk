#include "pch.h"

#include "Renderwerk/Graphics/VulkanGraphicsAdapter.h"

#include "Renderwerk/Graphics/VulkanContext.h"

FVulkanGraphicsAdapter::FVulkanGraphicsAdapter(const FVulkanContext& Context, const VkPhysicalDevice& InPhysicalDevice)
	: PhysicalDevice(InPhysicalDevice)
{
	vkGetPhysicalDeviceProperties(PhysicalDevice, &DeviceProperties);
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &MemoryProperties);
	AcquireExtensionsAndLayers();
	AcquireSupportedFeatures();
	QueryQueueMetadata(Context.Surface);
}

FVulkanGraphicsAdapter::~FVulkanGraphicsAdapter() = default;

bool8 FVulkanGraphicsAdapter::SupportsExtension(const char* ExtensionName) const
{
	return std::ranges::any_of(SupportedExtensions, [ExtensionName](const VkExtensionProperties& Extension)
	{
		return strcmp(Extension.extensionName, ExtensionName) == 0;
	});
}

bool8 FVulkanGraphicsAdapter::SupportsLayer(const char* LayerName) const
{
	return std::ranges::any_of(SupportedLayers, [LayerName](const VkLayerProperties& Layer)
	{
		return strcmp(Layer.layerName, LayerName) == 0;
	});
}

FVulkanSurfaceProperties FVulkanGraphicsAdapter::GetSurfaceProperties(const VkSurfaceKHR& Surface) const
{
	FVulkanSurfaceProperties SurfaceProperties = {};
	VkResult Result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &SurfaceProperties.Capabilities);
	VERIFY(Result == VK_SUCCESS, "Failed to get surface capabilities.");

	uint32 FormatCount = 0;
	Result = vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, nullptr);
	VERIFY(Result == VK_SUCCESS, "Failed to get surface formats.");
	SurfaceProperties.Formats.resize(FormatCount);
	Result = vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, SurfaceProperties.Formats.data());
	VERIFY(Result == VK_SUCCESS, "Failed to get surface formats.");

	uint32 PresentModeCount = 0;
	Result = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, nullptr);
	VERIFY(Result == VK_SUCCESS, "Failed to get present modes.");
	SurfaceProperties.PresentModes.resize(PresentModeCount);
	Result = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, SurfaceProperties.PresentModes.data());
	VERIFY(Result == VK_SUCCESS, "Failed to get present modes.");
	return SurfaceProperties;
}

void FVulkanGraphicsAdapter::AcquireExtensionsAndLayers()
{
	uint32 ExtensionCount = 0;
	VkResult Result = vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, nullptr);
	VERIFY(Result == VK_SUCCESS, "Failed to get extension count.");
	SupportedExtensions.resize(ExtensionCount);
	Result = vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, SupportedExtensions.data());
	VERIFY(Result == VK_SUCCESS, "Failed to get extensions.");

	uint32 LayerCount = 0;
	Result = vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, nullptr);
	VERIFY(Result == VK_SUCCESS, "Failed to get layer count.");
	SupportedLayers.resize(LayerCount);
	Result = vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, SupportedLayers.data());
	VERIFY(Result == VK_SUCCESS, "Failed to get layers.");
}

void FVulkanGraphicsAdapter::AcquireSupportedFeatures()
{
	Supported13Features = {};
	Supported13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	Supported12Features = {};
	Supported12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	Supported12Features.pNext = &Supported13Features;
	Supported11Features = {};
	Supported11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	Supported11Features.pNext = &Supported12Features;
	VkPhysicalDeviceFeatures2 SupportedFeatures2 = {};
	SupportedFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	SupportedFeatures2.pNext = &Supported11Features;

	vkGetPhysicalDeviceFeatures2(PhysicalDevice, &SupportedFeatures2);
	SupportedDeviceFeatures = SupportedFeatures2.features;
}

void FVulkanGraphicsAdapter::QueryQueueMetadata(const VkSurfaceKHR& Surface)
{
	uint32 QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
	TVector<VkQueueFamilyProperties> QueueFamilyProperties(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.data());

	uint8 BestTransferScore = 0;
	for (uint32 Index = 0; Index < QueueFamilyCount; ++Index)
	{
		uint8 CurrentTransferScore = 0;
		const VkQueueFamilyProperties QueueFamily = QueueFamilyProperties[Index];

		if (QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && QueueMetadata.GraphicsIndex == UINT32_MAX)
		{
			QueueMetadata.GraphicsIndex = Index;
			++CurrentTransferScore;
			if (QueueMetadata.PresentIndex == UINT32_MAX)
			{
				VkBool32 SupportsPresent = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, Index, Surface, &SupportsPresent);
				if (SupportsPresent)
				{
					QueueMetadata.PresentIndex = Index;
					++CurrentTransferScore;
				}
			}
		}
		if (QueueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT && QueueMetadata.ComputeIndex == UINT32_MAX)
		{
			QueueMetadata.ComputeIndex = Index;
			++CurrentTransferScore;
		}
		if (QueueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT && QueueMetadata.TransferIndex == UINT32_MAX)
		{
			if (CurrentTransferScore <= BestTransferScore)
			{
				BestTransferScore = CurrentTransferScore;
				QueueMetadata.TransferIndex = Index;
			}
		}
	}
	VERIFY(QueueMetadata.IsValid(), "Failed to find suitable queue family indices.");
}
