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

template <>
VkCommandPoolCreateInfo Vulkan::CreateStructure(VkCommandPoolCreateInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}

template <>
VkCommandBufferAllocateInfo Vulkan::CreateStructure(VkCommandBufferAllocateInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	Instance.pNext = nullptr;
	Instance.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	return Instance;
}

template <>
VkCommandBufferBeginInfo Vulkan::CreateStructure(VkCommandBufferBeginInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}

template <>
VkDebugUtilsLabelEXT Vulkan::CreateStructure(VkDebugUtilsLabelEXT Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	Instance.pNext = nullptr;
	return Instance;
}

template <>
VkSubmitInfo2 Vulkan::CreateStructure(VkSubmitInfo2 Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}

template <>
VkCommandBufferSubmitInfo Vulkan::CreateStructure(VkCommandBufferSubmitInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	Instance.pNext = nullptr;
	return Instance;
}

template <>
VkSemaphoreSubmitInfo Vulkan::CreateStructure(VkSemaphoreSubmitInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	Instance.pNext = nullptr;
	return Instance;
}

template <>
VkPresentInfoKHR Vulkan::CreateStructure(VkPresentInfoKHR Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	Instance.pNext = nullptr;
	return Instance;
}

template <>
VkImageViewCreateInfo Vulkan::CreateStructure(VkImageViewCreateInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}

template <>
VkSwapchainCreateInfoKHR Vulkan::CreateStructure(VkSwapchainCreateInfoKHR Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}

template <>
VkSemaphoreCreateInfo Vulkan::CreateStructure(VkSemaphoreCreateInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}

template <>
VkFenceCreateInfo Vulkan::CreateStructure(VkFenceCreateInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}

template <>
VkImageMemoryBarrier2 Vulkan::CreateStructure(VkImageMemoryBarrier2 Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	Instance.pNext = nullptr;
	return Instance;
}

template <>
VkPhysicalDeviceSynchronization2Features Vulkan::CreateStructure(VkPhysicalDeviceSynchronization2Features Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	Instance.pNext = nullptr;
	return Instance;
}

template <>
VkDependencyInfo Vulkan::CreateStructure(VkDependencyInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	Instance.pNext = nullptr;
	return Instance;
}

template <>
VkPhysicalDeviceDynamicRenderingFeatures Vulkan::CreateStructure(VkPhysicalDeviceDynamicRenderingFeatures Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	Instance.pNext = nullptr;
	return Instance;
}

template <>
VmaAllocatorCreateInfo Vulkan::CreateStructure(const VmaAllocatorCreateInfo Instance)
{
	return Instance;
}

template <>
VkBufferCreateInfo Vulkan::CreateStructure(VkBufferCreateInfo Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	Instance.pNext = nullptr;
	Instance.flags = 0;
	return Instance;
}

template <>
VmaAllocationCreateInfo Vulkan::CreateStructure(const VmaAllocationCreateInfo Instance)
{
	return Instance;
}

template <>
VkPhysicalDeviceBufferDeviceAddressFeatures Vulkan::CreateStructure(VkPhysicalDeviceBufferDeviceAddressFeatures Instance)
{
	Instance.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	Instance.pNext = nullptr;
	return Instance;
}
