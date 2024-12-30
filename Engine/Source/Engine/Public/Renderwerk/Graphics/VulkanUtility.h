#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

namespace VulkanUtility
{
	[[nodiscard]] uint32 GetInstanceVersion();
	[[nodiscard]] TVector<FString> GetInstanceLayers();
	[[nodiscard]] TVector<FString> GetInstanceExtensions();

	[[nodiscard]] TVector<FString> GetPhysicalDeviceLayers(VkPhysicalDevice PhysicalDevice);
	[[nodiscard]] TVector<FString> GetPhysicalDeviceExtensions(VkPhysicalDevice PhysicalDevice);

	[[nodiscard]] TVector<VkPhysicalDevice> GetPhysicalDevices(VkInstance Instance);
	[[nodiscard]] TVector<VkQueueFamilyProperties> GetQueueFamilyProperties(VkPhysicalDevice PhysicalDevice);

	[[nodiscard]] TVector<VkSurfaceFormatKHR> GetSurfaceFormats(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);
	[[nodiscard]] TVector<VkPresentModeKHR> GetPresentModes(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);
}
