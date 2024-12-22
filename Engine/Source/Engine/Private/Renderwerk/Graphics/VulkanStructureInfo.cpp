#include "pch.h"

#include "Renderwerk/Graphics/VulkanStructureInfo.h"

#include "Renderwerk/Graphics/GraphicsCommon.h"

template <>
VkApplicationInfo Vulkan::CreateStructure(VkApplicationInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	Instance.pNext = nullptr;
	return Instance;
}

template <>
VkInstanceCreateInfo Vulkan::CreateStructure(VkInstanceCreateInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}

template <>
VkWin32SurfaceCreateInfoKHR Vulkan::CreateStructure(VkWin32SurfaceCreateInfoKHR Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}

template <>
VkDeviceQueueCreateInfo Vulkan::CreateStructure(VkDeviceQueueCreateInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}

template <>
VkDeviceCreateInfo Vulkan::CreateStructure(VkDeviceCreateInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}
