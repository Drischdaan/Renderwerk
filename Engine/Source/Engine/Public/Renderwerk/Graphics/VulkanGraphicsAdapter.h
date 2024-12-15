#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include "Renderwerk/Graphics/GraphicsFwd.h"

#include <vulkan/vulkan_core.h>

struct ENGINE_API FVulkanQueueMetadata
{
	uint32 GraphicsIndex = UINT32_MAX;
	uint32 ComputeIndex = UINT32_MAX;
	uint32 TransferIndex = UINT32_MAX;
	uint32 PresentIndex = UINT32_MAX;

	[[nodiscard]] bool8 IsValid() const
	{
		return GraphicsIndex != UINT32_MAX && ComputeIndex != UINT32_MAX && TransferIndex != UINT32_MAX && PresentIndex != UINT32_MAX;
	}
};

struct ENGINE_API FVulkanSurfaceProperties
{
	VkSurfaceCapabilitiesKHR Capabilities;
	TVector<VkSurfaceFormatKHR> Formats;
	TVector<VkPresentModeKHR> PresentModes;
};

class ENGINE_API FVulkanGraphicsAdapter
{
public:
	FVulkanGraphicsAdapter(const FVulkanContext& Context, const VkPhysicalDevice& InPhysicalDevice);
	~FVulkanGraphicsAdapter();

	DELETE_COPY_AND_MOVE(FVulkanGraphicsAdapter);

public:
	[[nodiscard]] bool8 SupportsExtension(const char* ExtensionName) const;
	[[nodiscard]] bool8 SupportsLayer(const char* LayerName) const;

	[[nodiscard]] FVulkanSurfaceProperties GetSurfaceProperties(const VkSurfaceKHR& Surface) const;

public:
	[[nodiscard]] VkPhysicalDeviceProperties GetProperties() const { return DeviceProperties; }
	[[nodiscard]] VkPhysicalDeviceMemoryProperties GetMemoryProperties() const { return MemoryProperties; }

	[[nodiscard]] VkPhysicalDeviceFeatures GetSupportedDeviceFeatures() const { return SupportedDeviceFeatures; }
	[[nodiscard]] VkPhysicalDeviceVulkan11Features GetSupported11Features() const { return Supported11Features; }
	[[nodiscard]] VkPhysicalDeviceVulkan12Features GetSupported12Features() const { return Supported12Features; }
	[[nodiscard]] VkPhysicalDeviceVulkan13Features GetSupported13Features() const { return Supported13Features; }

	[[nodiscard]] FVulkanQueueMetadata GetQueueMetadata() const { return QueueMetadata; }

private:
	void AcquireExtensionsAndLayers();
	void AcquireSupportedFeatures();
	void QueryQueueMetadata(const VkSurfaceKHR& Surface);

private:
	VkPhysicalDevice PhysicalDevice;

	VkPhysicalDeviceProperties DeviceProperties;
	VkPhysicalDeviceMemoryProperties MemoryProperties;
	VkPhysicalDeviceFeatures SupportedDeviceFeatures;
	VkPhysicalDeviceVulkan11Features Supported11Features;
	VkPhysicalDeviceVulkan12Features Supported12Features;
	VkPhysicalDeviceVulkan13Features Supported13Features;

	TVector<VkExtensionProperties> SupportedExtensions;
	TVector<VkLayerProperties> SupportedLayers;

	FVulkanQueueMetadata QueueMetadata;
};
