#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

#include <vulkan/vulkan_core.h>

#define REQUIRED_VULKAN_INSTANCE_VERSION VK_API_VERSION_1_3

DECLARE_LOG_CHANNEL(LogVulkan);

class ENGINE_API FVulkanGraphicsApi
{
public:
	FVulkanGraphicsApi();
	~FVulkanGraphicsApi();

	DELETE_COPY_AND_MOVE(FVulkanGraphicsApi);

private:
	void AcquireApiVersion();
	void CreateAllocator();
	void CreateInstance();

#ifdef RW_ENABLE_GRAPHICS_VALIDATION
	void CreateDebugMessenger();
#endif

private:
	static void CheckExtensionAvailability(const TVector<const char*>& RequiredExtensions);
	[[nodiscard]] static bool8 IsExtensionSupported(const char* ExtensionName, const TVector<VkExtensionProperties>& AvailableExtensions);


	static void CheckLayerAvailability(const TVector<const char*>& RequiredLayers);
	[[nodiscard]] static bool8 IsLayerSupported(const char* LayerName, const TVector<VkLayerProperties>& AvailableLayers);

private:
	VkAllocationCallbacks* Allocator;

	uint32 ApiVersion;
	VkInstance Instance;

#ifdef RW_ENABLE_GRAPHICS_VALIDATION
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif
};
