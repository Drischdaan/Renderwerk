#pragma once

#include <volk.h>

namespace Vulkan
{
	template <typename TStructure>
	[[nodiscard]] TStructure CreateStructure(TStructure Instance = {})
	{
		return Instance;
	}

	template <>
	[[nodiscard]] inline VkApplicationInfo CreateStructure(VkApplicationInfo Instance)
	{
		Instance.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		Instance.pNext = nullptr;
		return Instance;
	}

	template <>
	[[nodiscard]] inline VkInstanceCreateInfo CreateStructure(VkInstanceCreateInfo Instance)
	{
		Instance.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		Instance.pNext = nullptr;
		Instance.flags = 0;
		return Instance;
	}

	template <>
	[[nodiscard]] inline VkWin32SurfaceCreateInfoKHR CreateStructure(VkWin32SurfaceCreateInfoKHR Instance)
	{
		Instance.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		Instance.pNext = nullptr;
		Instance.flags = 0;
		return Instance;
	}
}
