#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

#include "Renderwerk/Graphics/VulkanContext.h"

#define REQUIRED_VULKAN_INSTANCE_VERSION VK_API_VERSION_1_3

DECLARE_LOG_CHANNEL(LogVulkan);

struct ENGINE_API FVulkanGraphicsApiDesc
{
	TSharedPtr<FWindow> Window;
};

class ENGINE_API FVulkanGraphicsApi
{
public:
	FVulkanGraphicsApi(const FVulkanGraphicsApiDesc& InDescription);
	~FVulkanGraphicsApi();

	DELETE_COPY_AND_MOVE(FVulkanGraphicsApi);

public:
	[[nodiscard]] TVector<TSharedPtr<FVulkanGraphicsAdapter>> AcquireAdapters() const;
	[[nodiscard]] TSharedPtr<FVulkanGraphicsAdapter> GetSuitableAdapter() const;

	void Resize() const;

private:
	void AcquireApiVersion();
	void CreateAllocator();
	void CreateInstance();

#ifdef RW_ENABLE_GRAPHICS_VALIDATION
	void CreateDebugMessenger();
#endif

	void CreateSurface();
	void CreateDevice();
	void CreateSwapchain();

private:
	static void CheckExtensionAvailability(const TVector<const char*>& RequiredExtensions);
	[[nodiscard]] static bool8 IsExtensionSupported(const char* ExtensionName, const TVector<VkExtensionProperties>& AvailableExtensions);


	static void CheckLayerAvailability(const TVector<const char*>& RequiredLayers);
	[[nodiscard]] static bool8 IsLayerSupported(const char* LayerName, const TVector<VkLayerProperties>& AvailableLayers);

private:
	FVulkanGraphicsApiDesc Description;
	FVulkanContext Context;

	uint32 ApiVersion;

#ifdef RW_ENABLE_GRAPHICS_VALIDATION
	VkDebugUtilsMessengerEXT DebugMessenger;
#endif

	TSharedPtr<FVulkanGraphicsSwapchain> Swapchain;
};
